#ifndef TCP_HPP
#define TCP_HPP

#include <cstring>
#include <string>
#include <unistd.h>

class TCP {
    private:
        int tcp_socket;

    public:
        // Constructor
        TCP();

        /**
         * @brief Connects to a server
         * @param hostname The hostname of the server
         * @param port The port number
         * @return True if the connection was successful, false otherwise
        */
        bool connectToServer(const std::string& hostname, uint16_t port);

        /**
         * @brief Receives data from a TCP socket
         * @param tcp_socket The TCP socket
         * @return The received data
        */
        std::string receiveData(int tcp_socket);

        /**
         * @brief Sends data to a TCP socket
         * @param data The data to send
         * @return True if the data was sent successfully, false otherwise
        */
        bool sendData(const std::string& data);

        /**
         * @brief Sends a BYE message to the server
        */
        void sendByeMessage();

        /**
         * @brief Gets the TCP socket
         * @return The socket
        */
        int getSocket() const;

        /**
         * @brief Closes the connection
        */
        void closecon();

        /**
         * @brief Gracefully shuts down the TCP connection
         * @param tcp The TCP object
        */
        void gracefulShutdown(TCP& tcp);

};

#endif