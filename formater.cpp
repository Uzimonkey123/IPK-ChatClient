#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

#include "validator.hpp"
#include "strings.hpp"
#include "formater.hpp"

Validator validator;
Strings strings;

TextFormatter::TextFormatter() : formattedText(""), previousdName(""), dName("") {}

void TextFormatter::restorePreviousdName() {
    dName = previousdName; // Restore the previous display name
}

bool TextFormatter::formatTCPCommand(const std::string& command) {
    // Split the command into words
    std::vector<std::string> words = strings.split(command);
    
    // Check if the command is valid
    if (command.starts_with("/auth")) {
        if(handleAuthCommand(words)) {
            formattedText = "AUTH " + words[1] + " AS " + dName + " USING " + words[2];
            return true;
        }
    } else if (command.starts_with("/join")) {
        if(handleJoinCommand(words)) {
            formattedText = "JOIN " + words[1] + " AS " + dName;
            return true;
        }
    } else if (command.starts_with("/rename")) {
        return !handleRenameCommand(words);
    } else if (command.starts_with("/help")) {
        getHelp();
        return true;
    }

    return false;
}

bool TextFormatter::handleAuthCommand(const std::vector<std::string>& words) {
    // Check if the command has the correct number of arguments
    if (words.size() != 4) {
        displayError("Invalid command usage - AUTH");
        return false;
    }

    std::string userID = words[1];
    std::string secret = words[2];
    std::string newDisplayName = words[3];

    // Check if the arguments are valid, if not, display error
    if (!validator.isValidID(userID) || !validator.isValidDName(newDisplayName) || !validator.isValidSecret(secret)) {
        displayError("Invalid USERNAME / SECRET / DISPLAY_NAME.");
        return false;
    }

    // If the display name is different from the new display name, update it
    if (dName != newDisplayName) {
        previousdName = dName;
        dName = newDisplayName;
    }

    return true;
}

bool TextFormatter::handleJoinCommand(const std::vector<std::string>& words) {
    // Check if the command has the correct number of arguments
    if (words.size() != 2) {
        displayError("Invalid command usage - JOIN");
        return false;
    }

    return true;
}

bool TextFormatter::handleRenameCommand(const std::vector<std::string>& words) {
    // Check if the command has the correct number of arguments
    if (words.size() != 2) {
        displayError("Invalid command usage - RENAME");
        return false;
    }

    std::string newDisplayName = words[1];
    // Validate the given display name
    if (!validator.isValidDName(newDisplayName)) {
        displayError("Invalid DISPLAY_NAME.");
        return false;
    }

    if (dName != newDisplayName) {
        previousdName = dName;
        dName = newDisplayName;
    }

    return true;
}

void TextFormatter::displayError(const std::string& message) {
    std::cerr << "ERR: " << message << std::endl;
}

void TextFormatter::formatMSG(const std::string& message) {
    // Format TCP message
    formattedText = "MSG FROM " + dName + " IS " + message;
}

void TextFormatter::formatRecv(const std::string& message) {

    std::string upperMessage = strings.toUpperCase(message); // Convert the message to uppercase
    size_t pos = upperMessage.find(" IS "); // Find the position of " IS "
    std::string msg = message.substr(pos + 4); // Get the message part
    size_t endPos = msg.rfind("\r\n"); // Find the position of the CRLF sequence

    // If the CRLF sequence is found, remove it
    if (endPos != std::string::npos) {
        msg = msg.substr(0, endPos);
    }

    // Format MSG
    if(upperMessage.starts_with("MSG")) {
        std::string msgFrom = strings.getWords(message, 2); // Get the display name
        std::cout << msgFrom << ": " << msg << "\n";
        std::cout << std::flush;
    }

    // Format Success and Failure messages
    if(upperMessage.starts_with("REPLY OK")) {
        std::cerr << "Success: " << msg << "\n";
    }

    if(upperMessage.starts_with("REPLY NOK")) {
        std::cerr << "Failure: " << msg << "\n";
    }

    // Format ERR messages
    if(upperMessage.starts_with("ERR FROM")) {
        std::cerr << "ERR FROM " << strings.getWords(message, 2) << ": " << msg << "\n";
    }
}

void TextFormatter::formatError(const std::string& message) {
    formattedText = "ERR FROM " + dName + " IS " + message + "\r\n";
}

// Format UDP error message
void TextFormatter::formatUDPerror(const std::string& message, uint16_t messageID) {
    setupUDP(message, messageID, '\xFE');
}

std::string TextFormatter::getFormattedText() {
    return formattedText;
}

void TextFormatter::getHelp() {
    std::cout << "---Help---" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "/auth <USERNAME> <SECRET> <DISPLAY_NAME>" << std::endl;
    std::cout << "/join <CHANNEL>" << std::endl;
    std::cout << "/rename <DISPLAY_NAME>" << std::endl;
    std::cout << std::endl;
    std::cout << "For message sending, just type the message and press enter." << std::endl;
    std::cout << "/join, /rename and message sending available just after successful /auth." << std::endl;
}

std::string TextFormatter::setupUDP(std::string message, uint16_t messageID, char type) {
    // Setup the beginning of the UDP message
    message += type;
    message += static_cast<char>((messageID >> 8) & 0xFF);
    message += static_cast<char>(messageID & 0xFF);

    return message;
}

bool TextFormatter::formatUDPCommand(std::string message, uint16_t messageID) {

    std::string retMessage = "";
    std::vector<std::string> words = strings.split(message); // Split the message into words
    // Check for the command type
    if(message.starts_with("/auth")) {
        if(!handleAuthCommand(words)) {
            return false;
        }
        
        // Format the message correctly
        retMessage = setupUDP(retMessage, messageID, '\x02');
        retMessage += words[1] + '\x00';
        retMessage += words[3] + '\x00'; 
        retMessage += words[2] + '\x00';

    } else if(message.starts_with("/join")) {
        if(!handleJoinCommand(words)) {
            return false;
        }

        retMessage = setupUDP(retMessage, messageID, '\x03');
        retMessage += words[1] + '\x00';
        retMessage += dName + '\x00';

    } else if(message.starts_with("/rename")) {
        return !handleRenameCommand(words);

    } else { // If nop command, it is MSG
        retMessage = setupUDP(retMessage, messageID, '\x04');
        retMessage += dName + '\x00';
        retMessage += message + '\x00';
    }

    formattedText = retMessage;
    return true;
}

bool TextFormatter::formatUDPrecv(const std::string& message) {
    // Get the first byte of the received message
    signed char firstByte = static_cast<signed char>(message[0]);
    
    if(firstByte == '\x01') {
        // In case of REPLY, we need the third byte for success/failure
        unsigned char thirdByte = static_cast<unsigned char>(message[3]);

        const size_t headerSize = 6;
        std::string newMessage;

        if (message.size() > headerSize) {
            newMessage = message.substr(headerSize); // Get the message part

            size_t end = newMessage.find('\0');
            if (end != std::string::npos) {
                newMessage = newMessage.substr(0, end);
            }
        }

        // According to the third byte, format the outcome
        if(thirdByte == '\x01') {
            std::cerr << "Success: " << newMessage << std::endl;
        } else {
            std::cerr << "Failure: " << newMessage << std::endl;
        }

    } else if(firstByte == '\x04') {
        std::vector<std::string> messageParts;
        messageParts = strings.splitMessageParts(message); // Split the message into parts

        // If the message has more than 2 parts, display the message
        if(messageParts.size() > 1) {
            std::cout << messageParts[0] << ": " << messageParts[1] << std::endl;
        } else {
            std::cerr << "ERR: Insufficient parts in the message." << std::endl;
            return false;
        }

    } else if(firstByte == '\xFE') {
        // Same as with the message, just different print
        std::vector<std::string> messageParts;
        messageParts = strings.splitMessageParts(message);

        if(messageParts.size() > 1) {
            std::cerr << "ERR FROM " << messageParts[0] << ": " << messageParts[1] << std::endl;
        } else {
            std::cerr << "ERR: Insufficient parts in the message." << std::endl;
            return false;
        }
    }

    return true;
}