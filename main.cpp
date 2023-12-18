#include <iostream>
#include <thread>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>

const int PORT = 12345;
const int BUFFER_SIZE = 400;
const int MAX_NUM_SOCKET = 2;

void wait(uint64_t time, bool cond = true){
    uint64_t start = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch()).count();
    while((start + time) > std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch()).count() && cond);
}

bool check(int** field){
    return false;
}

void receiveMessages(int socket, int* clientsSocket, int*** fields) {
    char buffer[BUFFER_SIZE];
    bool win = false;

    while (!win) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Connection closed by the other user." << std::endl;
            break;
        }

        std::cout << "Received: " << buffer << std::endl;
        if(buffer[0] != '\\')
            continue;
        for(int i = 0; i < MAX_NUM_SOCKET; i++){
            if(clientsSocket[i] != socket){
                send(clientsSocket[i], buffer, strlen(buffer), 0);
                win = check(fields[MAX_NUM_SOCKET - 1 - i]);   
            }
        }
    }
}

void handleClient(int socket, int* clientsSocket, int* clientNum, int*** fields){
    std::cout << "Connection established" << std::endl;

    while(*clientNum < 2){
        char buffer[30] = "waiting the second user...";
        send(socket, buffer, strlen(buffer), 0);
        wait(5000, (*clientNum < 2));
    }
    
    char buffer[30] = "There are two user connected";
    send(socket, buffer, strlen(buffer), 0);
    
    std::thread receiveThread(receiveMessages, socket, clientsSocket, fields);

    receiveThread.join();

    close(socket);
}

int main(int argc, char*argv[]) {
    // -- initing variables --
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int clientNum = 0;
    std::thread* clients = new std::thread[MAX_NUM_SOCKET];
    int* clientsSocket = new int[MAX_NUM_SOCKET];
    int*** fields = new int**[2];

    for(int i = 0; i < 2; i++)
      fields[i] = new int*[10];

    for(int i = 0; i < 10; i++){
      fields[0][i] = new int[10];
      fields[1][i] = new int[10];
    }

    if (serverSocket == -1) {
        std::cerr << "Error creating socket." << std::endl;
        return -1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    if(argc < 2)
      serverAddress.sin_port = htons(PORT);
    else
      serverAddress.sin_port = htons(atoi(argv[1]));
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding socket." << std::endl;
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, 1) == -1) {
        std::cerr << "Error listening on socket." << std::endl;
        close(serverSocket);
        return -1;
    }
    
    // -- establishing connection --
    std::cout << "Waiting for the other user to connect..." << std::endl;

    sockaddr_in clientAddress{};
    socklen_t clientAddressSize = sizeof(clientAddress);
    int clientSocket = 0;
    while(clientNum < 2){
								clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddressSize);
								if (clientSocket == -1) {
												std::cerr << "Error accepting connection." << std::endl;
            continue;
								}
        
        // -- handling connection --
        clientsSocket[clientNum] = clientSocket;

        clients[clientNum] = std::thread(handleClient, clientSocket, clientsSocket, &clientNum, fields);
        clientNum++;
    }

    // -- deiniting --
    close(serverSocket);

    clients[0].join();
    clients[1].join();

    delete[] clientsSocket;
    delete[] clients;
    for(int i = 0; i < 10; i++){
      delete[] fields[0][i];
      delete[] fields[1][i];
    }
    for(int i = 0; i < 2; i++)
      delete[] fields[i];
    delete[] fields;
    return 0;
}
