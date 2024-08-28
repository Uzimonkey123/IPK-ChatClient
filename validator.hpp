#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <iostream>
#include <chrono>
#include <string>

class Validator {
    public:
        /**
         * @brief Check if the ID is valid
         * @param id The ID to check
         * @return True if the ID is valid, false otherwise
        */
        bool isValidID(const std::string& id);

        /**
         * @brief Check if the display name is valid
         * @param dname The display name to check
         * @return True if the display name is valid, false otherwise
        */
        bool isValidDName(const std::string& dname);

        /**
         * @brief Check if the secret is valid
         * @param secret The secret to check
         * @return True if the secret is valid, false otherwise
        */
        bool isValidSecret(const std::string& secret);
};

#endif