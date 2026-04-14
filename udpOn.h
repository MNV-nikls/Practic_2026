#pragma once

#include <QObject>
#include <QString>
#include <atomic>
#include <winsock2.h>

class UdpOn : public QObject {
    Q_OBJECT

public:
    explicit UdpOn(QObject* parent = nullptr, int port = 50005, const char* addres = "127.0.0.1");
    ~UdpOn();
    void stop();

public slots:
    void run();

signals:
    void dataParsed(uint32_t time, float value);
    void logMessage(const QString& msg);
    void finished();

private:
    std::atomic<bool> m_running; 
    SOCKET m_socket;
    const char* ADDR;
    int PORT;
};