#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QThread>
#include "udpOn.h"

class QtApp : public QWidget {
    Q_OBJECT

public:
    explicit QtApp(QWidget* parent = nullptr);
    ~QtApp();

private slots:
    void appendLog(const QString& msg);
    void handleData(uint32_t time, float value);

private:
    QTextEdit* logConsole;
    QThread* secThread;
    UdpOn* signaler;
};
