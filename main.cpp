#include <fcntl.h>
#include <thread>
#include <sys/select.h>
#include <signal.h>
#include <chrono>

#include "./tcp.hpp"
#include "./fsm.hpp"
#include "./parser.hpp"
#include "./udp.hpp"
#include "./commands.hpp"
#include "./lastMessage.hpp"

Parser parser; // Global variable to store the parsed arguments
std::atomic<bool> interruptReceived(false); // Global variable to store the interrupt signal
LastMessage lastMessage; // Global variable to store the last message sent in UDP

/**
 * @brief Signal handler for SIGINT
*/
void sigintHandler(int) {
    interruptReceived.store(true);
}


// ******************** TCP HANDLING FUNCTIONS ********************
/**
 * @brief Handles the receiving of TCP data from the server
 * @param tcp The TCP object
 * @param readfds The file descriptor set
 * @param fsm The FSM object
 * @param formatter The TextFormatter object
 * @return True if the operation was successful, false otherwise
*/
bool handleReceive(TCP& tcp, fd_set& readfds, FSM& fsm, TextFormatter& formatter) {
    if (FD_ISSET(tcp.getSocket(), &readfds)) { 
        std::string message = tcp.receiveData(tcp.getSocket());
        // "" indicates error in receiving data
        if(message == "") {
            return false;
        }
        if (!message.empty()) {
            // Check if the messsage is acceptable by the FSM
            fsm.ErrMessageCheck(message, parser.getTransport());

            // Handle the message with the FSM
            int errCheck = fsm.handleMessage(message, parser.getTransport());
            if(errCheck == -1) { // Unexpected message from server
                formatter.formatError("Unexpected message from server.");
                tcp.sendData(formatter.getFormattedText());
                return false;

            } else if(errCheck == -2) { // Received an error message
                formatter.formatRecv(message);
                return false;

            } else if(errCheck == -4) { // Ignore for reply in the open state
                return true;
            }

            // Format the message
            formatter.formatRecv(message);
        }
    }

    return true;
}

/**
 * @brief Handles the sending of TCP data to the server
 * @param tcp The TCP object
 * @param readfds The file descriptor set
 * @param fsm The FSM object
 * @param formatter The TextFormatter object
 * @return True if the operation was successful, false otherwise
*/
bool handleSend(TCP& tcp, fd_set& readfds, FSM& fsm, TextFormatter& formatter) {
    if (FD_ISSET(STDIN_FILENO, &readfds)) {
        Commands tcpParser;
        std::string data;
        // Read the input from the user
        if(std::getline(std::cin, data)) {
            if(data.empty()) { // Check if the input is empty
                std::cerr << "ERR: Empty input." << std::endl;
                return true;
            }
        } else {
            return true;
        }

        // Use a stringstream to split the data by new lines
        std::istringstream stream(data);
        std::string command;

        // Process each command
        while(std::getline(stream, command)) {
            if(!tcpParser.TCPcommands(command, formatter)) {
                continue; // Skip to the next command if the current one fails
            }
            command = tcpParser.getResult();    
            command += "\r\n"; // Add the CRLF sequence

            int errCheck = fsm.handleMessage(command, parser.getTransport());
            if(errCheck == -3) {
                std::cerr << "ERR: Invalid command, try using /help." << std::endl;
                formatter.restorePreviousdName();
                continue; // Skip to the next command if the current one fails
            }

            if(!tcp.sendData(command)) {
                return false; // Stop processing if sending fails
            }
        }
    }

    return true;
}

// ******************** UDP HANDLING FUNCTIONS ********************
/**
 * @brief Handles the sending of UDP data to the server
 * @param udp The UDP object
 * @param readfds The file descriptor set
 * @param messageID The message ID
 * @param formatter The TextFormatter object
 * @param fsm The FSM object
 * @return True if the operation was successful, false otherwise
*/
bool handleUDPsend(UDP& udp, fd_set& readfds, uint16_t& messageID, TextFormatter& formatter, FSM& fsm) {
    if (FD_ISSET(STDIN_FILENO, &readfds)) { // Check if there is input from the user
        std::string inputLine;
        std::string message;
        Commands udpParser;

        // Read the input from the user
        if(std::getline(std::cin, inputLine)) {
            if(inputLine.empty()) { // Check if the input is empty
                std::cerr << "ERR: Empty input." << std::endl;
                return true;
            }
        } else {
            return true;
        }

        // Process the input
        if(!udpParser.UDPcommands(inputLine, formatter, messageID)) {
            return true;
        }
        message = udpParser.getResult();

        int errCheck = fsm.handleMessage(message, parser.getTransport());
        if(errCheck == -3) {
            std::cerr << "ERR: Invalid command, try using /help." << std::endl;
            formatter.restorePreviousdName();
            return true;

        }
        
        // Send the message
        if(!udp.sendUDP(parser.getHostname(), message)) {
            return false;
        } else {
            // Set the last message sent
            fsm.resetConfirmation(); // Reset the confirmation flag
            lastMessage.message = message;
            lastMessage.time = std::chrono::steady_clock::now();
            lastMessage.waitingForConfirmation = true;
            lastMessage.retryCount = 0;
            
        }
    }   

    return true;
}

/**
 * @brief Handles the receiving of UDP data from the server
 * @param udp The UDP object
 * @param readfds The file descriptor set
 * @param formatter The TextFormatter object
 * @param fsm The FSM object
 * @param messageID The message ID
 * @return True if the operation was successful, false otherwise
*/
bool handleUDPrecv(UDP& udp, fd_set& readfds, TextFormatter& formatter, FSM& fsm, uint16_t messageID) {
    if (FD_ISSET(udp.getSocket(), &readfds)) {
        std::string result = udp.recvUDP(udp.getSocket()); // Receive the data
        // Get the first byte of the result
        unsigned char firstByte = static_cast<unsigned char>(result[0]);

        // "" indicates error in receiving data
        if(result == "") {
            formatter.formatUDPerror("Invalid message from server.", messageID);
            udp.sendUDP(parser.getHostname(), formatter.getFormattedText());
            return false;
        }

        // Check if the message is confirm, if no, process the message
        if(firstByte != 0x00) {
            // Send the confirmation
            udp.sendConfirm(parser.getHostname());

            // Check if the message has duplicate ID
            if(!udp.isDuplicate()) {
                fsm.ErrMessageCheck(result, parser.getTransport());
                int errCheck = fsm.handleMessage(result, parser.getTransport());
                if(errCheck == -1) {
                    formatter.formatUDPerror("Unexpected message from server.", messageID);
                    udp.sendUDP(parser.getHostname(), formatter.getFormattedText());
                    return false;

                } else if(errCheck == -2) { // Received an error message
                    formatter.formatUDPrecv(result);
                    return false;

                } else if(errCheck == -4) { // Ignore for reply in the open state
                    return true;
                }
            }

        } else {
            // Set the confirmation flag
            fsm.setConfirmation();
            lastMessage.waitingForConfirmation = false;
        }

        // Format the message if not duplicate
        if(!udp.isDuplicate()) {
            if(!formatter.formatUDPrecv(result)) {
                formatter.formatUDPerror("Invalid message from server.", messageID);
                udp.sendUDP(parser.getHostname(), formatter.getFormattedText());
                return false;
            }
        }
    }

    return true;
}

/**
 * @brief Sets up the protocol specifics for the given protocol
 * @param protocol The protocol to set up
 * @param tcp The TCP object
 * @param udp The UDP object
 * @param readfds The file descriptor set
 * @param maxfd The maximum file descriptor
*/
void setupProtocolSpecifics(const std::string& protocol, TCP& tcp, UDP& udp, fd_set& readfds, int& maxfd) {
    if(protocol == "udp") {
        FD_SET(udp.getSocket(), &readfds);
        maxfd = std::max(udp.getSocket(), STDIN_FILENO) + 1;
        udp.setPort(parser.getPort());

    } else if(protocol == "tcp") {
        FD_SET(tcp.getSocket(), &readfds);
        maxfd = std::max(tcp.getSocket(), STDIN_FILENO) + 1;
    }
}

/**
 * @brief Runs the client for the given protocol
 * @param tcp The TCP object
 * @param udp The UDP object
 * @param protocol The protocol to run
 * @return True if the operation was successful, false otherwise
*/
bool runProtocolts(TCP& tcp, UDP& udp, const std::string& protocol) {
    TextFormatter formatter;
    FSM fsm;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    bool exitFlag = false; // Flag to indicate the loop needs to exit
    bool returnFlag = false; // Flag to indicate the function needs to return with error or no
    signal(SIGINT, sigintHandler);
    uint16_t messageID = 0;
    lastMessage.retryCount = 0;

    // Set the timeout for the select
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 200000;
    int maxfd;

    // Set up the protocol specifics
    setupProtocolSpecifics(protocol, tcp, udp, readfds, maxfd);

    // Set the confirmation flag for TCP since it does not need confirmation
    if(protocol == "tcp") {
        fsm.setConfirmation();
    }

    // Main loop
    while(!exitFlag) {
        fd_set tmpfds = readfds;

        // Check if there is EOF, and if we are not waiting for a reply
        if (std::cin.eof() && !fsm.isWaitingForReply()) {
            break;
        }

        // Resend the last message if it is waiting for confirmation for too long
        if (protocol == "udp") {
            if(!lastMessage.resendLastMessage(udp, parser.getTimeout(), parser.getRetransmission(), parser)) {
                returnFlag = true; // Return error if we could not deliver message to server
                break;
            }
        }

        // Check if there is any data to read
        int ready = select(maxfd, &tmpfds, nullptr, nullptr, &timeout);
        if (ready == -1) {
            if (errno == EINTR) {// Check if the select was interrupted
                if (interruptReceived.load() && fsm.confirmPresent()) {
                    //usleep(1500000);
                    break;
                }
            } else {
                std::cerr << "ERR: Select error " << strerror(errno) << std::endl;
                returnFlag = true;
                break;
            }
        }

        // Handle the protocol receive and sending
        if (protocol == "tcp") {
            if (fsm.isWaitingForSend()) {
                if(!handleSend(tcp, tmpfds, fsm, formatter)) {
                    exitFlag = true;
                    returnFlag = true;
                }
            }

            if (!handleReceive(tcp, tmpfds, fsm, formatter)) {
                exitFlag = true;
                returnFlag = true;
            }

        } else {
            if (fsm.isWaitingForSend() && fsm.confirmPresent()) {
                if(!handleUDPsend(udp, tmpfds, messageID, formatter, fsm)) {
                    exitFlag = true;
                    returnFlag = true;
                
                }
            }
            
            if (!handleUDPrecv(udp, tmpfds, formatter, fsm, messageID)) {
                exitFlag = true;
                returnFlag = true;
            }
        }

        // Check for C-c
        if (interruptReceived.load() && fsm.confirmPresent() && !fsm.isWaitingForReply()) {
            //usleep(1500000);
            break;
        }
    }

    // Graceful shutdown
    if(protocol == "tcp") {
        tcp.gracefulShutdown(tcp);
        usleep(1500000);
        
    } else {
        udp.udpBye(udp, messageID, parser.getHostname());
        udp.recvUDP(udp.getSocket()); // Wait for the last confirmation
        close(udp.getSocket());
    }
    
    // Close all the file descriptors
    for (int fd = 0; fd < maxfd; ++fd) {
        if (FD_ISSET(fd, &readfds)) {
            close(fd);
        }
    }

    // Return false if we need to return with error
    if(returnFlag && !fsm.isServerSideBye()) {
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    //Error handling if hostname or transport is not present
    if (!parser.parseArgs(argc, argv)) {
        return 1;
    }

    // Print help if the help flag is present
    if (parser.isHelpPresent()) {
        parser.printHelp();
        return 0;
    }

    TCP tcp;
    UDP udp;

    if(parser.getTransport() == "tcp") {
        // Connect to the server for TCP
        if (!tcp.connectToServer(parser.getHostname(), parser.getPort())) {
            return 1;
        }

        if(!runProtocolts(tcp, udp, parser.getTransport())) {
            return 1;
        }

    } else if(parser.getTransport() == "udp") {
        // Create a UDP socket
        udp.createSocket();
        if(!runProtocolts(tcp, udp, parser.getTransport())) {
            return 1;
        }
    }

    return 0;
}