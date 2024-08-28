#ifndef LASTMESSAGE_HPP
#define LASTMESSAGE_HPP

#include <string>
#include <chrono>

#include "udp.hpp"
#include "parser.hpp"

class LastMessage {
    public:
        std::string message; // The last message sent
        std::chrono::steady_clock::time_point time; // The time the last message was sent
        bool waitingForConfirmation; // Whether the client is waiting for a confirmation
        int retryCount; // The number of times the message has been resent

        /**
         * @brief Resends the last message sent by the client
         * @param udp The UDP object
         * @param timeout The timeout value
         * @param maxRetries The maximum number of retries
         * @param parser The parser object
         * @return True if the message was resent, false otherwise
        */
        bool resendLastMessage(UDP &udp, uint16_t timeout, int maxRetriesm, Parser &parser);
};
#endif