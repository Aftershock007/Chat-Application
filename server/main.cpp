#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <thread>
#include<vector>
#include<algorithm>

using namespace std;

#pragma comment(lib,"ws2_32.lib")

bool initialization(){
    WSADATA data;
    return (WSAStartup(MAKEWORD(2,2), &data) == 0);
}

void interactWithClient(SOCKET clientSocket, vector<SOCKET>& clients){
    // send/receive
    cout<<"Client connected"<<endl;
    char buffer[100000];
    while(true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(bytesReceived <=0){
            cout<<"Client disconnected"<<endl;
            break;
        }
        string message(buffer, bytesReceived);
        string name = message.substr(0, message.find(':'));
        string msg = message.substr(message.find(':') + 2);
        cout << "Message from "<< name << ": " << msg << endl;
        for(SOCKET client:clients){
            if(client != clientSocket) {
                send(client, message.c_str(), (int) message.length(), 0);
            }
        }
    }
    auto it = find(clients.begin(), clients.end(), clientSocket);
    if(it != clients.end()){
        clients.erase(it);
    }
    closesocket(clientSocket);
}

int main() {
    // Initialization
    if(!initialization()) {
        cout<<"Winsock initialization failed"<<endl;
        return 1;
    }
    // Socket creation
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listenSocket == INVALID_SOCKET) {
        cout<<"Socket creation failed"<<endl;
        return 1;
    }
    // Address structure creation
    int PORT = 12345;
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    // Convert the IP to binary
    if(inet_pton(AF_INET,_T("0.0.0.0"), &serverAddress.sin_addr) != 1){
        cout<<"Setting address structure failed"<<endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    // Bind
    if(bind(listenSocket,reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR){
        cout<<"Bind failed"<<endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    // Listen
    if(listen(listenSocket, SOMAXCONN) == SOCKET_ERROR){
        cout<<"Listen failed"<<endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    cout << "Server has started listening on port: " << PORT << endl;
    // accept
    vector<SOCKET> clients;
    while(true) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            cout << "Invalid client socket" << endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        clients.push_back(clientSocket);
        // Parallelly accepting different client requests
        thread t1(interactWithClient, clientSocket, std::ref(clients));
        t1.detach();
    }
}
