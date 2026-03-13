#include <iostream>
#include <string>
#include <cstdint>
#include <cstring>  
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 50006
#define BUFFER_SIZE 1024

int main() {

    const char* ip_address = "127.0.0.1";

    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in servaddr, cliaddr;

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "Error: WSAStartup " << iResult << std::endl;
        return -1;
    }

    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd  == INVALID_SOCKET) {
        std::cerr << "Error: failed create socket - " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    ZeroMemory(&servaddr, sizeof(servaddr));
    ZeroMemory(&cliaddr, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip_address, &servaddr.sin_addr) <= 0) {
        std::cerr << "Error: IP address format - " << ip_address << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }


    if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        std::cerr << "Error: Bind - " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }

    std::cout << "UDP Server started !!!" << std::endl;


    while (true) {

        int len = int(sizeof(cliaddr));
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&cliaddr, &len);

        if (n == 8) {
            uint32_t timestamp;
            float value;
            std::memcpy(&timestamp, buffer, sizeof(uint32_t));
            std::memcpy(&value, buffer + sizeof(uint32_t), sizeof(float));

            std::cout << "Received: " << timestamp << " ;     Value: " << value << std::endl;
        }
        else if (n > 0) {
            std::cout << "Received more bytes: " << n << std::endl;
        }
    }

    closesocket(sockfd);
    WSACleanup();

    return 0;
}
