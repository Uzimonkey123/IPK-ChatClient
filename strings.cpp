#include <iostream>
#include <sstream>

#include "strings.hpp"

std::string Strings::getWords(const std::string& input, int position) {
    std::istringstream iss(input);
    std::string word;
    std::string dname;

    iss >> word; // Get the first word

    for (int i = 0; i < position; ++i) { // Loop through the words
        iss >> word;
    }

    return word;
}

std::vector<std::string> Strings::split(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> words;
    std::string word;

    while (iss >> word) { // Loop through the words
        words.push_back(word);
    }

    return words;
}

std::string Strings::toUpperCase(const std::string& input) {
    std::string output = input; // Copy the input string to output
    for(char& c : output) {
        c = std::toupper(static_cast<unsigned char>(c)); // Convert each character to uppercase
    }
    return output;
}

int Strings::wordCount(const std::string& command) {
    std::vector<std::string> words = split(command); // Split the command into words
    return words.size();
}

std::vector<std::string> Strings::splitMessageParts(const std::string& input) {
    std::vector<std::string> parts;
    // Ensure we skip the initial bytes only if the input is long enough
    size_t startPos = (input.size() > 3) ? 3 : input.size();
    while (startPos < input.size()) {
        size_t endPos = input.find('\x00', startPos);
        if (endPos == std::string::npos) {
            endPos = input.size();
        }
        // Extract part if it's not empty
        if (endPos > startPos) {
            parts.push_back(input.substr(startPos, endPos - startPos));
        }
        startPos = endPos + 1; // Move past the '\x00' delimiter
    }
    return parts;
}