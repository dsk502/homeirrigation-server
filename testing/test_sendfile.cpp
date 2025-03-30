#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BLOCK_SIZE 1024 * 1024 // Each block is 1MB

// Ensure all data is sent
int sendAll(int sock, const char* buf, int len) {
    int total = 0;
    while (total < len) {
        int sent = send(sock, buf + total, len - total, 0);
        if (sent == -1) return -1;
        total += sent;
    }
    return total;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding the socket to the port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    // Accepting a connection
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Connection accepted." << std::endl;

    // Open the file to be sent
    std::ifstream file("example.txt", std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open file" << std::endl;
        close(new_socket);
        close(server_fd);
        return -1;
    }

    // Get file size
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Send file size to client
    if (sendAll(new_socket, (char*)&fileSize, sizeof(fileSize)) == -1) {
        std::cerr << "Failed to send file size" << std::endl;
        file.close();
        close(new_socket);
        close(server_fd);
        return -1;
    }

    // Send file content in chunks
    char buffer[BLOCK_SIZE];
    while (fileSize > 0) {
        int chunkSize = (fileSize > BLOCK_SIZE) ? BLOCK_SIZE : fileSize;
        file.read(buffer, chunkSize);
        if (sendAll(new_socket, buffer, chunkSize) == -1) {
            std::cerr << "Failed to send file chunk" << std::endl;
            file.close();
            close(new_socket);
            close(server_fd);
            return -1;
        }
        fileSize -= chunkSize;
    }

    file.close();
    close(new_socket);
    close(server_fd);
    std::cout << "File sent successfully." << std::endl;

    return 0;
}