#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "handleMsg.h"

const int BUFFER_SIZE = 1024;

void receiveMessages(int socket) {
    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Connection closed by the other user." << std::endl;
            break;
        }

        std::cout << "Received: " << buffer << std::endl;
    }
}

void sendMessages(int socket) {
    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        std::cout << "Type a message: ";
        std::cin.getline(buffer, sizeof(buffer));

        send(socket, buffer, strlen(buffer), 0);
    }
}
