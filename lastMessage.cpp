#include <iostream>
#include <string>

#include "lastMessage.hpp"

bool LastMessage::resendLastMessage(UDP &udp, uint16_t timeout, int maxRetries, Parser &parser) {
    if (this->waitingForConfirmation) {
        auto now = std::chrono::steady_clock::now(); // Get the current time

        // Calculate the elapsed time
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->time);

        // If the retry count is greater than the maximum number of retries, return false
        if(this->retryCount >= maxRetries) {
            return false;
        }

        // If the elapsed time is greater than or equal to the timeout, resend the message
        if (elapsed.count() >= timeout) {
            // Resend the message
            udp.sendUDP(parser.getHostname(), this->message);
            this->time = std::chrono::steady_clock::now(); // Update the time
            retryCount++;
        }
    }

    return true;
}