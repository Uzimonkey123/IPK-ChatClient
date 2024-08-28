#ifndef UDP_HPP
#define UDP_HPP

#include <unistd.h>
#include <iomanip>
#include <unordered_set>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>

class UDP {
    private:
        int udp_socket;
        uint16_t messageID;
        uint16_t port;
        struct sockaddr_in server_addr;
        int counter; // Counter just to know the first connection
        std::unordered_set<uint16_t> idList;
        bool server_addr_initialized = false;
        bool duplicate; // Flag to check if the message is a duplicate

    public:

        // Constructor
        UDP();

        /**
         * @brief Creates a UDP socket
        */
        void createSocket();

        /**
         * @brief Gets the UDP socket
         * @return The socket
        */
        int getSocket();

        /**
         * @brief Sets the port number
         * @param port The port number
        */
        void setPort(uint16_t port);

        /**
         * @brief Checks if the message is a duplicate
         * @return True if the message is a duplicate, false otherwise
        */
        bool isDuplicate();

        /**
         * @brief Sends a UDP message
         * @param hostname The hostname of the server
         * @param message The message to send
         * @return True if the message was sent successfully, false otherwise
        */
        bool sendUDP(const std::string& hostname, const std::string& message);

        /**
         * @brief Receives a UDP message
         * @param udp_socket The UDP socket
         * @return The received message
        */
        std::string recvUDP(int udp_socket);

        /**
         * @brief Sends a confirmation message
         * @param hostname The hostname of the server
        */
        void sendConfirm(const std::string& hostname);

        /**
         * @brief Sends a BYE message
         * @param hostname The hostname of the server
        */
        void udpBye(UDP& udp, uint16_t messageID, const std::string hostname);

        /**
         * @brief Gets the message ID
         * @return The message ID
        */
        uint16_t getMessageID();
};

#endif