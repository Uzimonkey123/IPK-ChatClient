#include <iostream>
#include <chrono>
#include <unistd.h>

#include "strings.hpp"
#include "fsm.hpp"

FSM::FSM() : state(State::START),
            waitingForSend(true),
            receivedConfirmation(true) {}
            
// Function to block sending messages
bool FSM::isWaitingForSend() const {
    return waitingForSend;
}

// Function to know if we are waiting for reply,
// used in the exception for sending BYE while waiting
bool FSM::isWaitingForReply() const {
    return state == State::AUTH || state == State::REPLY;
}

void FSM::ErrMessageCheck(const std::string& message, const std::string& protocol) {
    Strings strings;
    // Get the first byte of the message
    unsigned char firstByte = static_cast<signed char>(message[0]);

    std::string upperMessage = strings.toUpperCase(message); // Convert the message to uppercase

    switch(state) { // Switch statement to check the state of the FSM
        // States behaviour separated by the protocol that is used
        case State::START:
            if(protocol == "tcp" || protocol == "TCP") {
                // If the message in the start is something else then error, set the state to ERR
                if(upperMessage.find("ERR FROM") == std::string::npos) {
                    waitingForSend = true;
                    state = State::ERR;
                }
            } else if(protocol == "udp" || protocol == "UDP") {
                if(firstByte != static_cast<unsigned char>(0xFE)) {
                    waitingForSend = true;
                    state = State::ERR;
                }
            }
            break;

        case State::AUTH:
            if(protocol == "tcp" || protocol == "TCP") {
                // Waiting for reply or error from the server, anything else after /auth command is unexpected
                if (upperMessage.find("REPLY OK") == std::string::npos && upperMessage.find("REPLY NOK") == std::string::npos &&
                    upperMessage.find("ERR FROM") == std::string::npos) {
                    state = State::ERR;
                    waitingForSend = true;
                }
            } else if(protocol == "udp" || protocol == "UDP") {
                if(firstByte != 0x01 && firstByte != static_cast<unsigned char>(0xFE)) {
                    state = State::ERR;
                    waitingForSend = true;
                }
            }
            break;

        case State::OPEN:
            // In open we can expect anything from the server from: error, msg, bye
            // else we set the state to ERR. Reply is unaccepted here, because we are not expecting one.
            if(protocol == "tcp" || protocol == "TCP") {
                if(upperMessage.find("MSG") == std::string::npos && upperMessage.find("ERR FROM") == std::string::npos &&
                   upperMessage.find("REPLY") == std::string::npos) {
                    state = State::ERR;
                    waitingForSend = true;
                }
            } else if(protocol == "udp" || protocol == "UDP") {
                if(firstByte != 0x04 && firstByte != static_cast<unsigned char>(0xFE) && firstByte != 0x01) {
                    state = State::ERR;
                    waitingForSend = true;
                }
            }

        default:
            break;
    }

}

int FSM::handleMessage(const std::string& message, const std::string& protocol) {
    Strings strings;
    // Get the first and third byte of the message
    unsigned char firstByte = static_cast<signed char>(message[0]);
    // Get the third byte of the message for REPLY OK/NOK
    signed char thirdByte = static_cast<signed char>(message[3]);

    // Convert the message to uppercase
    std::string upperMessage = strings.toUpperCase(message);

    switch (state) {
        case State::START:
            // In the start we can send only /auth command, anything else is unaccepted
            if(protocol == "tcp" || protocol == "TCP") {
                if (upperMessage.starts_with("AUTH")) {
                    state = State::AUTH;
                    waitingForSend = false;
                } else {
                    return -3;
                }
            } else if(protocol == "udp" || protocol == "UDP") {
                if (firstByte == '\x02') {
                    state = State::AUTH;
                    waitingForSend = false;
                } else {
                    return -3;
                }
            }
            break;

        case State::AUTH:
            // Depending on the reply, we move to the next state
            if(protocol == "tcp" || protocol == "TCP") {
                if (upperMessage.starts_with("REPLY OK")) {
                    waitingForSend = true;
                    state = State::OPEN;

                } else if(upperMessage.starts_with("REPLY NOK")) {
                    waitingForSend = true;
                    state = State::START;
                } else if(upperMessage.starts_with("ERR FROM")) {
                    return -2;
                }
            } else if(protocol == "udp" || protocol == "UDP") {
                if(firstByte == 0x01) {
                    if (thirdByte == 0x01) {
                        state = State::OPEN;
                        waitingForSend = true;
                    } else if(thirdByte == 0x00) {
                        waitingForSend = true;
                        state = State::START;
                    }
                } else if(firstByte == static_cast<unsigned char>(0xFE)){
                    return -2;
                }
            }
            break;

        case State::OPEN:
            if(protocol == "tcp" || protocol == "TCP") {
                if (upperMessage.starts_with("JOIN")) {
                    state = State::REPLY;
                    waitingForSend = false;

                } else if (upperMessage.starts_with("MSG FROM")) {
                    waitingForSend = true;
                } else if(upperMessage.starts_with("ERR FROM")) {
                    return -2; // Error from the server, -2 so we can send bye
                } else if(upperMessage.starts_with("REPLY")) {
                    return -4; // Reply in the open state is unaccepted, since we are not waiting for any (we ignore it)
                } else {
                    return -3; // Invalid command
                }

            } else if(protocol == "udp" || protocol == "UDP") {
                if (firstByte == 0x03) {
                    state = State::REPLY;
                    waitingForSend = false;
                } else if (firstByte == 0x04 || firstByte == static_cast<unsigned char>(0xFF)) {
                    waitingForSend = true;
                } else if(firstByte == static_cast<unsigned char>(0xFE)){
                    return -2; // Error from the server, -2 so we can send bye
                } else if(firstByte == 0x01) {
                    return -4; // Reply in the open state is unaccepted, since we are not waiting for any (we ignore it)
                } else {
                    return -3; // Invalid command
                }
            }
            break;

        case State::REPLY:
            // In reply state, we are just waiting for either error or reply from the server
            if(protocol == "tcp" || protocol == "TCP") {
                if (upperMessage.starts_with("REPLY OK") || upperMessage.starts_with("REPLY NOK")) {
                    waitingForSend = true;
                    state = State::OPEN;
                } else if(upperMessage.starts_with("ERR FROM")) {
                    return -2;
                }
            } else if(protocol == "udp" || protocol == "UDP") {
                if (firstByte == 0x01) {
                    state = State::OPEN;
                    waitingForSend = true;
                }
            }
            break;

        case State::ERR:

            // If we already in error state, we check if the server kicked us out, so we don't send BYE
            if(upperMessage.starts_with("BYE") || firstByte == 0xFF) {
                serverSideBye = true;
                return -2;
            }

            // else, we got an unexpected message from the server
            std::cerr << "ERR: Unexpected message from server." << std::endl;
            waitingForSend = true;
            return -1;
    }

    return 0;
}

bool FSM::confirmPresent() {
    return receivedConfirmation;
}

void FSM::resetConfirmation() {
    receivedConfirmation = false;
}

void FSM::setConfirmation() {
    receivedConfirmation = true;
}

bool FSM::isStart() const {
    return state == State::START;
}

bool FSM::isErr() const {
    return state == State::ERR;
}

bool FSM::isServerSideBye() const {
    return serverSideBye;
}