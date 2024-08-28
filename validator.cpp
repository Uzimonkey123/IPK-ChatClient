#include "validator.hpp"

bool Validator::isValidID(const std::string& id) {
    if (id.empty() || id.length() > 20) { // Check if the ID is empty or too long
        return false;
    }

    for (char c : id) {
        // Check if the character is not a letter, digit or '-'
        if (!std::isalpha(c) && !std::isdigit(c) && c != '-') {
            return false;
        }
    }

    return true;
}

bool Validator::isValidDName(const std::string& dname) {
    if (dname.empty() || dname.length() > 20) { // Check if the display name is empty or too long
        return false;
    }

    for (char c : dname) {
        // Check if the character is not printable
        if (!std::isprint(c)) {
            return false;
        }
    }

    return true;
}

bool Validator::isValidSecret(const std::string& secret) {
    if (secret.empty() || secret.length() > 128) { // Check if the secret is empty or too long
        return false;
    }

    for (char c : secret) {
        // Check if the character is not a letter, digit or '-'
        if (!std::isalpha(c) && !std::isdigit(c) && c != '-') {
            return false;
        }
    }

    return true;
}
