#include <iostream>
#include <netdb.h>
#include <sys/types.h>

#include "udp.hpp"

UDP::UDP() : udp_socket(-1), messageID(0), port(0), duplicate(false) 
{
    memset(&server_addr, 0, sizeof(server_addr));
}

void UDP::createSocket() {
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0) { // Check for errors
        std::cerr << "ERR: Error creating socket." << std::endl;
        return;
    }   
}

int UDP::getSocket() {
    return udp_socket;
}

void UDP::setPort(uint16_t port) {
    this->port = port;
}

bool UDP::isDuplicate() {
    return duplicate;
}

bool UDP::sendUDP(const std::string& hostname, const std::string& message) {
    if (!server_addr_initialized) {
        struct addrinfo hints, *result; // Structs for address info
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET; // IPv4
        hints.ai_socktype = SOCK_DGRAM; // UDP

        // Get address info
        int status = getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints, &result);
        if (status != 0) {
            std::cerr << "ERR: getaddrinfo error." << std::endl;
            return false;
        }

        // Copy the address info to the server_addr struct
        memcpy(&server_addr, result->ai_addr, sizeof(server_addr));
        server_addr_initialized = true;

        // Free the address info
        freeaddrinfo(result);
    }

    // Send the message
    ssize_t bytes_sent = sendto(udp_socket, message.c_str(), message.size(), 0,
                                (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bytes_sent < 0) { // Check for errors
        std::cerr << "ERR: Error sending data." << std::endl;
        return false;
    }

    return true;
}

std::string UDP::recvUDP(int udp_socket) {
    char buffer[5000];
    struct sockaddr_in sender_addr; // Temporary structure for the sender's address
    socklen_t sender_addr_len = sizeof(sender_addr);

    // Receive data
    ssize_t bytes_rx = recvfrom(udp_socket, buffer, sizeof(buffer), 0, 
                                (struct sockaddr *)&sender_addr, &sender_addr_len);
    if (bytes_rx > 0) {
        // Check if the message is from the server
        buffer[bytes_rx] = '\0'; // Null-terminate the received data
    } else if (bytes_rx < 0) { // Check for errors
        std::cerr << "ERR: Data receiving error." << std::endl;
        return "";
    }

    if(bytes_rx < 3) {
        std::cerr << "ERR: Received message is too short." << std::endl;
        return "";
    }

    // Copy the sender's address to the server address
    server_addr.sin_addr = sender_addr.sin_addr;
    server_addr.sin_port = sender_addr.sin_port;

    // Check if the message is a duplicate
    signed char firstByte = buffer[0];
    if(firstByte != 0x00) {
        messageID = (static_cast<uint8_t>(buffer[1]) << 8) | static_cast<uint8_t>(buffer[2]);

        // Add the message ID to the list
        if(idList.find(messageID) != idList.end()) {
            duplicate = true;
        } else {
            idList.insert(messageID);
            duplicate = false;
        }
    }

    return std::string(buffer, bytes_rx);
}

void UDP::sendConfirm(const std::string& hostname) {
    std::string confirm;
    // Append the initial byte
    confirm += '\x00';
    confirm += static_cast<char>((messageID >> 8) & 0xFF);
    confirm += static_cast<char>(messageID & 0xFF);
    sendUDP(hostname, confirm);
}

void UDP::udpBye(UDP& udp, uint16_t messageID, const std::string hostname) {
    std::string message;
    message += '\xFF'; // Append the BYE byte
    message += static_cast<char>((messageID >> 8) & 0xFF);
    message += static_cast<char>(messageID & 0xFF);

    udp.sendUDP(hostname, message);
}

uint16_t UDP::getMessageID() {
    return messageID;
}