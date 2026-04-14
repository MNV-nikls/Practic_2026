#include "udpOn.h"
#include <ws2tcpip.h>
#include <cstring>

#define BUFFER_SIZE 1024

UdpOn::UdpOn(QObject* parent, int port, const char* addres) : QObject(parent), m_running(false), m_socket(INVALID_SOCKET), PORT(port), ADDR(addres) {}

UdpOn::~UdpOn() {
    stop();
}

void UdpOn::stop() {
    m_running = false;
    if (m_socket != INVALID_SOCKET) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

void UdpOn::run() {
    m_running = true;
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        emit logMessage("Error: WSAStartup failed");
        emit finished();
        return;
    }

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET) {
        emit logMessage("Error: failed to create socket");
        WSACleanup();
        emit finished();
        return;
    }

    struct sockaddr_in servaddr;
    ZeroMemory(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    const char* ip_address = ADDR;

    if (bind(m_socket, (const struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        emit logMessage(QString("Error: failed to bind. Code: %1").arg(WSAGetLastError()));
        stop();
        WSACleanup();
        emit finished();
        return;
    }

    emit logMessage(QString("Winsock UDP Server started! Listening on %1:%2...").arg(ip_address).arg(PORT));

    char buffer[BUFFER_SIZE];
    struct sockaddr_in cliaddr;

    while (m_running) {
        int len = sizeof(cliaddr);
        int n = recvfrom(m_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&cliaddr, &len);

        if (!m_running) break;

        if (n == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAEINTR || err == WSAENOTSOCK) {
                break;
            }
            emit logMessage(QString("Error receiving data: %1").arg(err));
            continue;
        }

        if (n == 8) {
            uint32_t time;
            float value;
            std::memcpy(&time, buffer, sizeof(uint32_t));
            std::memcpy(&value, buffer + sizeof(uint32_t), sizeof(float));

            emit dataParsed(time, value);
        }
        else if (n > 0) {
            emit logMessage(QString("Receiv %1 bytes.").arg(n));
        }
    }

    stop();
    WSACleanup();
    emit finished();
}
