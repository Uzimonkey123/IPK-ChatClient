#include <iostream>

#include "parser.hpp"

bool Parser::parseArgs(int argc, char *argv[]) {

    //Temporary bools to check if the arguments are present
    bool transport_available = false;
    bool hostname_available = false;
    int tmp_timeout;

    for(int i = 0; i < argc; i++) {
        switch(argv[i][1]) {
            case 't':
                if(i + 1 < argc) {
                    transport = argv[i+1];
                    transport_available = true;
                    // Convert transport to lowercase
                        for (char &c : transport) {
                            c = std::tolower(static_cast<unsigned char>(c));
                        }
                }
                i++;
                break;
            case 's':
                if(i + 1 < argc) { // Check if the hostname is present
                    hostname = argv[i+1]; // Get the hostname
                    hostname_available = true;
                }
                i++;
                break;
            case 'p':
                if(i + 1 < argc) {
                    port = atoi(argv[i+1]);
                }
                i++;
                break;
            case 'd':
                if(i + 1 < argc) {
                    tmp_timeout = atoi(argv[i+1]);
                    timeout = static_cast<uint16_t>(tmp_timeout);
                }
                i++;
                break;
            case 'r':
                if(i + 1 < argc) {
                    retransmission = atoi(argv[i+1]);
                }
                i++;
                break;
            case 'h':
                help_present = true;
                return true;
        }
    }

    // We must have transport protocol and hostname specified
    if(!transport_available || !hostname_available) {
        std::cerr << "ERR: Transport and hostname are required arguments" << std::endl;
        return false;
    }

    return true;
}

Parser::Parser() :
            help_present(false),
            port(4567),
            timeout(250),
            retransmission(3) {}
            
// Getters
std::string Parser::getTransport() {
    return transport;
}

std::string Parser::getHostname() {
    return hostname;
}

uint16_t Parser::getPort() {
    return port;
}

uint16_t Parser::getTimeout() {
    return timeout;
}

uint8_t Parser::getRetransmission() {
    return retransmission;
}

bool Parser::isHelpPresent() {
    return help_present;
}

void Parser::printHelp() {
    std::cout << "Usage: ./client [OPTIONS]" << std::endl;
    std::cout << "  -t <transport>  Transport protocol (tcp/udp)" << std::endl;
    std::cout << "  -s <hostname>   Server hostname" << std::endl;
    std::cout << "  -p <port>       Server port" << std::endl;
    std::cout << "  -d <timeout>    Timeout in milliseconds" << std::endl;
    std::cout << "  -r <retrans>    Number of retransmissions" << std::endl;
    std::cout << "  -h              Display this help and exit" << std::endl;
}
