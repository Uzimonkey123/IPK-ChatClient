#ifndef FSM_HPP
#define FSM_HPP

#include <chrono>
#include <unistd.h>
#include <string>

class FSM {
    public:
        // Enum class to represent the state of the FSM
        enum class State {
            START,
            AUTH, // Auth state is used when waiting for a reply just from /auth command
            OPEN,
            REPLY, // Reply state is used when waiting for a reply just from /join command
            ERR
        };

        State state;

    private:
        bool waitingForSend; // Allowing to block sending messages
        bool receivedConfirmation = false; // Bool variable to know if we got confirm for a sent message
        bool serverSideBye = false; // Bool variable to know if the server sent a BYE message

    public:
        // Constructor
        FSM();

        /**
         * @brief Returns if we are waiting for a send
        */
        bool isWaitingForSend() const;

        /**
         * @brief Returns if we are waiting for a reply
        */
        bool isWaitingForReply() const;

        /**
         * @brief Function to check if the message is accepted in the given state
         * @param message The message to check
         * @param protocol The protocol used
        */
        void ErrMessageCheck(const std::string& message, const std::string& protocol);

        /**
         * @brief Function to handle the state of the FSM
         * @param message The message to handle
         * @param protocol The protocol used
        */
        int handleMessage(const std::string& message, const std::string& protocol);

        /**
         * @brief Function to tell if there is a confirmation
        */
        bool confirmPresent();

        /**
         * @brief Function to reset the confirmation variable to false
        */
        void resetConfirmation();

        /**
         * @brief Function to set the confirmation variable to true
        */
        void setConfirmation();

        /**
         * @brief Function to check if the state is START
        */
        bool isStart() const;

        /**
         * @brief Function to check if the state is ERR
        */
        bool isErr() const;

        /**
         * @brief Function to check if the server sent a BYE message
        */
        bool isServerSideBye() const;
};

#endif