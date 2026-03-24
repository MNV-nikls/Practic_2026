#include "Qtapp.h"
#include <QVBoxLayout>

QtApp::QtApp(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Practic 2026");
    resize(700, 500);

    logConsole = new QTextEdit(this);
    logConsole->setReadOnly(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(logConsole);
    setLayout(layout);

    secThread = new QThread(this);
    signaler = new UdpOn();
    signaler->moveToThread(secThread);

    connect(secThread, &QThread::started, signaler, &UdpOn::run);

    connect(signaler, &UdpOn::logMessage, this, &QtApp::appendLog);

    connect(signaler, &UdpOn::dataParsed, this, &QtApp::handleData);

    connect(signaler, &UdpOn::finished, secThread, &QThread::quit);

    connect(signaler, &UdpOn::finished, signaler, &UdpOn::deleteLater);

    secThread->start();
}

QtApp::~QtApp() {
    if (signaler) {
        signaler->stop();
    }

    if (secThread) {
        secThread->quit();
        secThread->wait();
    }
}

void QtApp::appendLog(const QString& msg) {
    logConsole->append(msg);
}

void QtApp::handleData(uint32_t time, float value) {
    QString msg = QString("Received data. time: %1        Value: %2")
        .arg(time)
        .arg(double(value));
    logConsole->setText(msg);
}
