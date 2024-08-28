#ifndef STRINGS_HPP
#define STRINGS_HPP

#include <string>
#include <vector>

class Strings {
    public:
        /**
         * @brief Get the words from the input string
         * @param input The input string
         * @param position The position of the word
         * @return The word at the position
        */
        std::string getWords(const std::string& input, int position);

        /**
         * @brief Split the input string into words
         * @param str The input string
         * @return A vector of words
        */
        std::vector<std::string> split(const std::string& str);

        /**
         * @brief Convert the input string to uppercase
         * @param input The input string
         * @return The uppercase string
        */
        std::string toUpperCase(const std::string& input);

        /**
         * @brief Count the number of words in the input string
         * @param command The input string
         * @return The number of words
        */
        int wordCount(const std::string& command);

        /**
         * @brief Split the message into parts used for UDP error and MSG handling
         * @param message The input message
         * @return A vector of message parts
        */
        std::vector<std::string> splitMessageParts(const std::string& str);
};

#endif