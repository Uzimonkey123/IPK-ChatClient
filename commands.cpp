#include <iostream>

#include "commands.hpp"

std::string Commands::getResult() {
    return result; // Return the result of the last command
}

bool Commands::TCPcommands(const std::string& command, TextFormatter& formatter) {
    // Check for the specific command and format it
    if(command.starts_with("/auth")) {
        if(!formatter.formatTCPCommand(command)) {
            return false;
        }
        result = formatter.getFormattedText();

    } else if(command.starts_with("/join")) {
        if(!formatter.formatTCPCommand(command)) {
            return false;
        }
        result = formatter.getFormattedText();

    } else if(command.starts_with("/rename")) {
        return formatter.formatTCPCommand(command);

    } else if(command.starts_with("/help")) {
        formatter.getHelp();
        return false;

    } else if(command.starts_with("/")) {
        std::cerr << "ERR: Invalid command." << std::endl;
        return false;

    } else { // If the string is not command, consider it as message
        formatter.formatMSG(command);
        result = formatter.getFormattedText();
    }

    return true;
}

bool Commands::UDPcommands(const std::string& command, TextFormatter& formatter, uint16_t& messageID) {
    // Check for the specific command and format it
    if(command.starts_with("/auth")) {
        if(!formatter.formatUDPCommand(command, messageID++)) {
            return false;
        }
        result = formatter.getFormattedText();

    } else if(command.starts_with("/join")) {
        if(!formatter.formatUDPCommand(command, messageID++)) {
            return false;
        }
        result = formatter.getFormattedText();

    } else if(command.starts_with("/rename")) {
        return formatter.formatUDPCommand(command, messageID);

    } else if(command.starts_with("/help")) {
        formatter.getHelp();
        return false;

    } else if(command.starts_with("/")) {
        std::cerr << "ERR: Invalid command." << std::endl;
        return false;

    } else { // If the string is not command, consider it as message
        formatter.formatUDPCommand(command, messageID++);
        result = formatter.getFormattedText();
    }

    return true;
}