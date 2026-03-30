#include "Qtapp.h"
#include <QVBoxLayout>

QtApp::QtApp(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Practic 2026");
    resize(700, 500);

    logConsole = new QTextEdit(this);
    logConsole->setReadOnly(true);

    kihConsole = new QTextEdit(this);
    kihConsole->setReadOnly(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(logConsole);
    layout->addWidget(kihConsole);
    setLayout(layout);

    secThread = new QThread(this);
    signaler = new UdpOn();
    signaler->moveToThread(secThread);

    kihThread = new QThread(this);
    kih = new Kih();
    kih->moveToThread(kihThread);

    connect(secThread, &QThread::started, signaler, &UdpOn::run);
    connect(signaler, &UdpOn::logMessage, this, &QtApp::appendLog);
    connect(signaler, &UdpOn::dataParsed, this, &QtApp::handleData);
    connect(signaler, &UdpOn::finished, secThread, &QThread::quit);
    connect(signaler, &UdpOn::finished, signaler, &UdpOn::deleteLater);
    secThread->start();

    connect(kihThread, &QThread::started, kih, &Kih::run);
    connect(this, &QtApp::toFilter, kih, &Kih::filter);
    connect(kih, &Kih::kihResult, this, &QtApp::kihFilterData);
    kihThread->start();
}

QtApp::~QtApp() {
    if (signaler) {
        signaler->stop();
    }

    if (secThread) {
        secThread->quit();
        secThread->wait();
    }

    if (kih) {
        kih->stop();
    }

    if (kihThread) {
        kihThread->quit();
        kihThread->wait();
    }
}

void QtApp::appendLog(const QString& msg) {
    logConsole->append(msg);
}

void QtApp::handleData(uint32_t time, float value) {
    QString msg = QString("Received data. time: %1        Value: %2")
        .arg(time)
        .arg(value);
    logConsole->setText(msg);
    emit toFilter(time, value);
}

void QtApp::kihFilterData(uint32_t time, float value) {
    QString msg = QString("KIH data. time: %1             Value: %2")
        .arg(time)
        .arg(value);
    kihConsole->setText(msg);
}
