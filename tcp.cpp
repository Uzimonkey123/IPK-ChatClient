#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>

#include "tcp.hpp"

TCP::TCP() : tcp_socket(-1) {}

bool TCP::connectToServer(const std::string& hostname, uint16_t port) {
    struct addrinfo hints, *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Get address info
    int status = getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints, &result);
    if (status != 0) {
        std::cerr << "ERR: getaddrinfo error." << std::endl;
        return false;
    }

    // Copy the address info to the server_addr struct
    struct sockaddr_in *server_addr = (struct sockaddr_in *)result->ai_addr;

    // Create a socket
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket < 0) {
        std::cerr << "ERR: Error creating socket." << std::endl;
        freeaddrinfo(result);
        return false;
    }

    // Connect to the server
    if (connect(tcp_socket, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        std::cerr << "ERR: Error connecting to the server." << std::endl;
        freeaddrinfo(result);
        return false;
    }

    // Free the address info
    freeaddrinfo(result);
    return true;
}

std::string TCP::receiveData(int tcp_socket) {
    std::string data;
    char buffer[1];
    bool foundCR = false; // Flag to check for carriage return

    while (true) {
        // Receive data
        ssize_t bytes_rx = recv(tcp_socket, buffer, sizeof(buffer), 0);
        if (bytes_rx > 0) {
            data.append(buffer, bytes_rx); // Append received byte(s) to data

            if (buffer[0] == '\r') { // Check for carriage return
                foundCR = true;
            } else if (buffer[0] == '\n' && foundCR) { // Check for newline if carriage return was found
                // \r\n sequence found, add it and exit loop
                data.append(buffer, bytes_rx);
                break;
            } else {
                foundCR = false; // Reset if any other character is found
            }
        } else if (bytes_rx < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) { // Check if the socket is non-blocking
                continue;
            }
            std::cerr << "ERR: Data receiving error." << std::endl;
            return "";
        } else {
            return "";
        }
    }

    return data;
}

bool TCP::sendData(const std::string& data) {
    // Check if the socket is valid
    if (tcp_socket < 0) {
        std::cerr << "ERR: No connection to the server." << std::endl;
        return false;
    }

    // Send data
    ssize_t bytes_tx = send(tcp_socket, data.c_str(), data.size(), 0);
    if (bytes_tx < 0) {
        std::cerr << "ERR: Error sending data to the server." << std::endl;
        return false;
    }

    return true;
}

void TCP::sendByeMessage() {
    sendData("BYE\r\n");
}

int TCP::getSocket() const {
    return tcp_socket;
}

void TCP::closecon() {
    if (tcp_socket >= 0) {
        close(tcp_socket);
    }
}

void TCP::gracefulShutdown(TCP& tcp) {
    tcp.sendByeMessage();
    tcp.closecon();
}