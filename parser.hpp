#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

class Parser {
    private:
        bool help_present; // Flag to check if the help argument is present
        std::string transport;
        std::string hostname;
        uint16_t port;
        uint16_t timeout;
        uint8_t retransmission;

    public:
        // Constructor
        Parser();

        /**
         * @brief Parse the arguments from the command line
        */
        bool parseArgs(int argc, char *argv[]);

        /**
         * @brief Return the transport protocol
        */
        std::string getTransport();

        /**
         * @brief Return the hostname
        */
        std::string getHostname();

        /**
         * @brief Return the port
        */
        uint16_t getPort();

        /**
         * @brief Return the timeout for udp
        */
        uint16_t getTimeout();

        /**
         * @brief Return the retransmission for udp
        */
        uint8_t getRetransmission();

        /**
         * @brief Check if the help argument is present
        */
        bool isHelpPresent();

        /**
         * @brief Print the help message
        */
        void printHelp();
};

#endif