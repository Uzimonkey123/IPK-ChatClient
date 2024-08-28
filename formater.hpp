#ifndef FORMATER_HPP
#define FORMATER_HPP

#include <string>
#include <vector>
#include "validator.hpp"
#include "strings.hpp"

class TextFormatter {
    private:
        std::string formattedText; // The formatted text from the passed message
        std::string previousdName; // Previous display name ready to be restored if necessary
        std::string dName; // Current display name

    public:
        // Constructor
        TextFormatter();

        /**
         * @brief Restore the previous display name in case of a bad command usage
        */
        void restorePreviousdName();

        /**
         * @brief Format the command for TCP
         * 
         * @param command Command to be formatted
         * @return true if the command was formatted successfully
         * @return false if the command was not formatted successfully
        */
        bool formatTCPCommand(const std::string& command);

        /**
         * @brief handles the /auth command and checks for its validity
         * @param words vector of strings containing the command and its arguments
        */
        bool handleAuthCommand(const std::vector<std::string>& words);

        /**
         * @brief handles the /join command and checks for its validity
         * @param words vector of strings containing the command and its arguments
        */
        bool handleJoinCommand(const std::vector<std::string>& words);

        /**
         * @brief handles the /rename command and checks for its validity
         * @param words vector of strings containing the command and its arguments
        */
        bool handleRenameCommand(const std::vector<std::string>& words);

        /**
         * @brief Display an error message
         * 
         * @param message Error message to be displayed
        */
        void displayError(const std::string& message);

        /**
         * @brief Format a message for TCP
         * 
         * @param message Message to be formatted
        */
        void formatMSG(const std::string& message);
        
        /**
         * @brief Format received messages for TCP
         * 
         * @param message Message to be formatted
        */
        void formatRecv(const std::string& message);

        /**
         * @brief Format an error message for TCP
         * 
         * @param message Error message to be formatted
        */
        void formatError(const std::string& message);

        /**
         * @brief Format an error message for UDP
         * 
         * @param message Error message to be formatted
         * @param messageID Message ID to be used
        */
        void formatUDPerror(const std::string& message, uint16_t messageID);

        /**
         * @brief Get the formatted text
         * 
         * @return std::string Formatted text
        */
        std::string getFormattedText();

        /**
         * @brief Get the mid-program help message
        */
        void getHelp();

        /**
         * @brief Setup the beginning of the UDP message
         * 
         * @param message Message to be formatted
         * @param messageID Message ID to be used
         * @param type Type of the message
         * @return std::string Formatted message
        */
        std::string setupUDP(std::string message, uint16_t messageID, char type);

        /**
         * @brief Format a UDP command
         * 
         * @param message Message to be formatted
         * @param messageID Message ID to be used
         * @return true if the command was formatted successfully
         * @return false if the command was not formatted successfully
        */
        bool formatUDPCommand(std::string message, uint16_t messageID);

        /**
         * @brief Format an UDP received message
         * 
         * @param message Message to be formatted
        */
        bool formatUDPrecv(const std::string& message);
};

#endif