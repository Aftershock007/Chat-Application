#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>

using namespace std;

#pragma comment(lib,"ws2_32.lib")

bool initialization(){
    WSADATA data;
    return (WSAStartup(MAKEWORD(2,2), &data) == 0);
}

void SendsMessage(SOCKET s){
    cout<<"Enter your chat name: "<<endl;
    string name;
    getline(cin,name);
    string message;
    while(true){
        getline(cin, message);
        string msg = name;
        msg += ": " + message;
        int bytesSend = send(s, msg.c_str(), (int)(message.length() + name.length() + 2),0);
        if(bytesSend == SOCKET_ERROR){
            cout<<"Error sending message"<<endl;
            break;
        }
        if(message == "exit"){
            cout<<"Stopping the application"<<endl;
            break;
        }
    }
    closesocket(s);
    WSACleanup();
}

void ReceivesMessage(SOCKET s){
    char buffer[100000];
    int receiveLength;
    while(true){
        receiveLength = recv(s, buffer, sizeof(buffer), 0);
        if(receiveLength <= 0){
            cout<<"Disconnected from the server"<<endl;
            break;
        }else{
            string message = string(buffer, receiveLength);
            cout<<message<<endl;
        }
    }
    closesocket(s);
    WSACleanup();
}

int main() {
    // Initialization
    if(!initialization()) {
        cout<<"Winsock initialization failed"<<endl;
        return 1;
    }
    // Socket creation
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == INVALID_SOCKET){
        cout<<"Socket creation failed"<<endl;
        return 1;
    }
    // Address structure creation
    string serveraddress = "127.0.0.1";
    int PORT = 12345;
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    // Convert the IP to binary
    if(inet_pton(AF_INET, serveraddress.c_str(), &serverAddress.sin_addr) != 1){
        cout<<"Setting address structure failed"<<endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }
    // Connect
    if(connect(s, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR){
        cout<<"Not able to connect to server"<<endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }
    cout<<"Successfully connected to server"<<endl;

    // send/receive parallelly
    thread senderthread(SendsMessage, s);
    thread receiverthread(ReceivesMessage, s);

    senderthread.join();
    receiverthread.join();
    return 0;
}