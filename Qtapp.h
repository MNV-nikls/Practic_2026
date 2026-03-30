#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QThread>
#include "udpOn.h"
#include "kih-alg.h"

class QtApp : public QWidget {
    Q_OBJECT

public:
    explicit QtApp(QWidget* parent = nullptr);
    ~QtApp();

private slots:
    void appendLog(const QString& msg);
    void handleData(uint32_t time, float value);
    void kihFilterData(uint32_t time, float value);

signals:
    void toFilter(uint32_t time, float value);

private:
    QTextEdit* logConsole;
    QTextEdit* kihConsole;
    QTextEdit* bihConsole;
    QThread* secThread;
    QThread* kihThread;
    UdpOn* signaler;
    Kih* kih;
};
