#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "include/networking/networking_thread.hpp"

void server_func() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char* response = "Hello from server";

    // 创建TCP套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址和端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // 绑定套接字到地址和端口
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 开始监听
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    // 接受客户端连接
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connected." << std::endl;

    // 循环接收和发送消息
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // 清空缓冲区
        int valread = read(new_socket, buffer, 1024);   //The data from client will be in buffer
        if (valread <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break;  // 客户端断开连接
        }
        //Process the data from buffer
        std::cout << "Message from client: " << buffer << std::endl;

        // 向客户端发送响应
        send(new_socket, response, strlen(response), 0);
        std::cout << "Response sent to client." << std::endl;
    }

    // 关闭套接字
    close(new_socket);
    close(server_fd);

    return;
}