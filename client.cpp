#include <iostream>
#include <WinSock2.h>
#include <thread>
#include<string>
#include <WS2tcpip.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

bool Initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMsg(SOCKET s) {
    cout << "Enter your chat name:\n";
    string name;
    getline(cin, name);
    string message;

    while (true) {
        getline(cin, message);
        string msg = name + ": " + message;  
        int bytesSent = send(s, msg.c_str(), msg.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            cout << "Unable to send message.\n";
            break;
        }
        if (message == "quit") { 
            cout << "Exiting the program.\n";
            break;
        }
    }
    closesocket(s);
    WSACleanup();
}

void ReceiveMessage(SOCKET s) {
    char buffer[4096];
    int recvLength;

    while (true) {
        recvLength = recv(s, buffer, sizeof(buffer), 0); 
        if (recvLength <= 0) {
            cout << "Error receiving message.\n";
            break;
        }
        buffer[recvLength] = '\0'; 
        cout << buffer << endl;  
    }
    closesocket(s);
    WSACleanup();
}

int main() {
    if (!Initialize()) {
        cout << "Initialization failed.\n";
        return 1;
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        cout << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    string servaddr = "127.0.0.1";
    int port = 12345;
    sockaddr_in serveraddr = {};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);

    if (inet_pton(AF_INET, servaddr.c_str(), &serveraddr.sin_addr) <= 0) {
        cout << "Invalid address.\n";
        closesocket(s);
        WSACleanup();
        return 1;
    }

   
    if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "Error connecting to server.\n";
        closesocket(s); 
        WSACleanup();
        return 1;
    }

    cout << "Connection successful.\n";

    // Start sender and receiver threads
    thread senderThread(SendMsg, s);
    thread receiverThread(ReceiveMessage, s);

   
    senderThread.join();  
    receiverThread.join();  

    return 0;  
}
