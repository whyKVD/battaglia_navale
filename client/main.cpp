#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include "../handleMsg.h"
const int PORT = 12345;

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return -1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    // Change "127.0.0.1" to the IP address of the machine running the server.
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid server address." << std::endl;
        close(clientSocket);
        return -1;
    }

    if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::cerr << "Error connecting to the server." << std::endl;
        close(clientSocket);
        return -1;
    }

    std::cout << "Connected to the server. You can start typing messages." << std::endl;

    std::thread receiveThread(receiveMessages, clientSocket);
    std::thread sendThread(sendMessages, clientSocket);

    receiveThread.join();
    sendThread.join();

    close(clientSocket);

    return 0;
}
