#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "handleMsg.h"

const int BUFFER_SIZE = 400;
const int MAX_NUM_COLUMN_CHAT = 20;

void printField(int*** fields){
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            std::cout << fields[0][i][j] << "  ";
        }
        std::cout << " |   ";
        for(int j = 0; j < 10; j++){
            std::cout << fields[1][i][j] << "  ";
        }
        std::cout << std::endl;
    }
}

void receiveMessages(int socket) {
    char buffer[BUFFER_SIZE];
    int*** fields = new int**[2];
  
    for(int i = 0; i < 2; i++)
        fields[i] = new int*[10];
    for(int i = 0; i < 10; i++){
        fields[0][i] = new int[10];
        fields[1][i] = new int[10];
    }
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(socket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Connection closed by the other user." << std::endl;
            break;
        }

        if(buffer[0] != '\\'){
            continue;
        }
        int flag = 1;
        char* cmd;
        while(buffer[flag] != ' ' && buffer[flag] != '\0'){
            flag++;
        }
        cmd = new char[flag];
        for(int i = 1; i < flag; i++)
            cmd[i - 1] = buffer[i];
        cmd[flag - 1] = '\0';
        if(strcmp(cmd, "mv") == 0)
            printField(fields);
        else{
            for(int i = flag; buffer[i] != '\0'; i++)
                std::cout << buffer[i];
        }
        std::cout << std::endl;
{}
    }
    for(int i = 0; i < 10; i++){
        delete[] fields[0][i];
        delete[] fields[1][i]; 
    }
    for(int i = 0; i < 2; i++)
        delete[] fields[i];
    delete[] fields;
}

void sendMessages(int socket) {
    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        std::cout << "Type a message: ";
        std::cin.getline(buffer, sizeof(buffer));
       
        buffer[strlen(buffer)] = '\n';
        buffer[strlen(buffer)] = '\r'; 
        send(socket, buffer, strlen(buffer), 0);
    }
}
