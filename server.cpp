#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>


using namespace std;



#pragma comment(lib,"ws2_32.lib")
/*
	//init winsoc lib

	// create the socket 


	//get ip and port 


	//bind the ip and port with the socket 


	//listen on the socket 



	//accept 


	//recv and send 


*/


bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void InteractWithClient(SOCKET clientSocket,vector<SOCKET>&clients) {
	//send/recv

	cout << "client connected\n";

	char buffer[4056];

	while (1) {


		int bytesrecd = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesrecd <= 0) {
			cout << "client disconnect boss\n";
			break;

		}
		string message(buffer, bytesrecd);
		cout << "message from client:  " << message << endl;
		for (auto client : clients) {
			if(client != clientSocket)
			send(client, message.c_str(), message.length(), 0);
		}
	}
	auto it = find(clients.begin(), clients.end(), clientSocket);

	if (it != clients.end()) {
		clients.erase(it);
	}


	closesocket(clientSocket);
 


}
int main() {

	if (!Initialize()) {
		cout << "winsoc initialization failed\n";
		return 1;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	int port = 12345;
	if (listenSocket == INVALID_SOCKET) {
		cout << "socket creation failed\n";
		return 1;
	}
			

	//creating address structure 
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(12345);

	//convert the ip address (0.0.0.0) put it inside the sin_family in binary format 
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "setting address structure failed\n";
		closesocket(listenSocket);
		return 1;
	}

	//binding 

	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "bind failed your majesty\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//listen				  //max values of clients that can be put if queue to listen one by one 
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "listen error boss\n";
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}

	cout << "server has started listen on port: " << port << "\n";
	

	//accept
	// The accept function takes three parameters:
// 1. listenSocket: The socket descriptor on which the server is listening for connections.
// 2. addr (optional): A pointer to a sockaddr structure where the client’s address will be stored.
//    - If nullptr is passed, the server ignores the client’s address.
//    - To retrieve the client’s address, provide a pointer to a sockaddr structure (e.g., sockaddr_in for IPv4).
//      Example: struct sockaddr_in clientAddr;
// 3. addrlen (optional): A pointer to a socklen_t variable that initially holds the size of the addr structure.
//    - If nullptr is passed, the server ignores the length of the client’s address.
//    - To retrieve the length, provide a pointer to a socklen_t variable, which will be updated with the actual size.
//
// Example usage to retrieve client information:
// struct sockaddr_in clientAddr;
// socklen_t clientAddrSize = sizeof(clientAddr);
// int clientSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);

// Without retrieving client address and length information:

	vector<SOCKET>clients;
	while (1) {
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET) {
			cout << "inalvid client socket\n";
		}
		clients.push_back(clientSocket);


		thread t1(InteractWithClient, clientSocket,ref(clients));
		cout << "client connected\n";
		t1.detach();
	}

	 

	 


	 
	closesocket(listenSocket);


	WSACleanup();

	//cout << "yo\n";
	return 0;
}
