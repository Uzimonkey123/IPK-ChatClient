#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include "./formater.hpp"

class Commands {
    private:
        std::string result; // Result of the last command

    public:
        /**
         * @brief Get the result of the last command
        */
        std::string getResult();

        /**
         * @brief Parse the command and format it for TCP
         * 
         * @param command Command to be parsed
         * @param formatter Formatter object
         * @return true if the command was parsed successfully
         * @return false if the command was not parsed successfully
        */
        bool TCPcommands(const std::string& command, TextFormatter& formatter);

        /**
         * @brief Parse the command and format it for UDP
         * 
         * @param command Command to be parsed
         * @param formatter Formatter object
         * @param messageID Message ID to be used
         * @return true if the command was parsed successfully
         * @return false if the command was not parsed successfully
        */
        bool UDPcommands(const std::string& command, TextFormatter& formatter, uint16_t& messageID);
};

#endif