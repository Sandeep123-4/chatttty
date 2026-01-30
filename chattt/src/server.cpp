#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>

std::vector<int> clients;
std::mutex clients_mutex;

void broadcast(const std::string &message, int sender_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int client_fd : clients) {
        if (client_fd != sender_fd) send(client_fd, message.c_str(), message.size(), 0);
    }
}

void handle_client(int client_fd) {
    char buffer[1024];
    while (true) {
        int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        broadcast(buffer, client_fd);
    }
    close(client_fd);
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
}

int main() {
    int port = 12345;
    if (getenv("PORT")) port = std::stoi(getenv("PORT"));

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);
    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd >= 0) {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_fd);
            std::thread(handle_client, client_fd).detach();
        }
    }

    close(server_fd);
    return 0;
}


