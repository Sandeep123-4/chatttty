// client.cpp
#include <iostream>
#include <thread>
#include <string>
#include <arpa/inet.h> // For Linux. For Windows use winsock2.h and WSAStartup.
#include <unistd.h>    // For close()

void receive_messages(int sock) {
    char buffer[1024];
    while (true) {
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        std::cout << buffer << std::endl;
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = inet_addr("YOUR_SERVER_IP"); // Replace with Render public IP

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return 1;
    }

    std::thread(receive_messages, sock).detach();

    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") break;
        send(sock, message.c_str(), message.size(), 0);
    }

    close(sock);
    return 0;
}
