#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void sendFile(const char* filePath, int socket) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "无法打开文件" << std::endl;
        return;
    }

    char buffer[BUFFER_SIZE];
    while (file.read(buffer + 1, BUFFER_SIZE - 1)) {
        buffer[0] = 0x03; // 文件数据包标记
        int bytesToSend = file.gcount() + 1;
        if (send(socket, buffer, bytesToSend, 0) < 0) {
            std::cerr << "发送失败" << std::endl;
            break;
        }
    }

    // 发送文件结束标记
    char endMarker = 0xFF;
    send(socket, &endMarker, sizeof(endMarker), 0);

    file.close();
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    sendFile("example.txt", new_socket);

    // 关闭套接字
    close(new_socket);
    close(server_fd);

    return 0;
}