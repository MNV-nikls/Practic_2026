#include "Qtapp.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <ws2tcpip.h>

QtApp::QtApp(QWidget* parent) : QWidget(parent) {
    setWindowTitle("Practic 2026");
    resize(1000, 800);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    setWindowTitle("Winsock UDP Server & Sender (Qt6)");
    resize(800, 600);

    btnStart = new QPushButton("Start Listening", this);
    btnStop = new QPushButton("Stop Listening", this);
    btnStop->setEnabled(false);

    inputTarget = new QDoubleSpinBox(this);
    inputTarget->setRange(-1000000.0, 1000000.0); 
    inputTarget->setDecimals(2);                  
    inputTarget->setValue(0.0);                   

    btnSendTarget = new QPushButton("Send Target", this);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(btnStart);
    buttonsLayout->addWidget(btnStop);

    QVBoxLayout* ipRecieveLayout = new QVBoxLayout();
    QVBoxLayout* ipSendLayout = new QVBoxLayout();
    QVBoxLayout* targetLayout = new QVBoxLayout();

    ipRecieveInput = new QLineEdit(this);
    ipSendInput = new QLineEdit(this);
    portRecieveInput = new QSpinBox(this);
    portSendInput = new QSpinBox(this);

    ipRecieveInput->setText(QString("127.0.0.1"));
    ipSendInput->setText(QString("127.0.0.1"));
    portRecieveInput->setValue(50006);
    portRecieveInput->setRange(0, 100000);
    portSendInput->setValue(50005);
    portSendInput->setRange(0, 100000);

    ipRecieveLayout->addWidget(new QLabel("Recieve in (IP/Port):", this));
    ipRecieveLayout->addWidget(ipRecieveInput);
    ipRecieveLayout->addWidget(portRecieveInput);

    ipSendLayout->addWidget(new QLabel("Send to (IP/Port):", this));
    ipSendLayout->addWidget(ipSendInput);
    ipSendLayout->addWidget(portSendInput);

    targetLayout->addWidget(new QLabel("Target:", this));
    targetLayout->addWidget(inputTarget);
    

    series = new QLineSeries();
    lineK = new QLineSeries();
    lineB = new QLineSeries();

    series->setColor(Qt::gray);
    lineK->setColor(Qt::red);
    lineB->setColor(Qt::blue);

    chart = new QChart();
    chart->addSeries(series);
    chart->addSeries(lineK);
    chart->addSeries(lineB);
    chart->setTitle("Live UDP Data");
    chart->legend()->hide();

    axisX = new QValueAxis();
    axisX->setTitleText("Timestamp");
    axisX->setLabelFormat("%i");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    lineK->attachAxis(axisX);
    lineB->attachAxis(axisX);

    axisY = new QValueAxis();
    axisY->setTitleText("Value");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    lineK->attachAxis(axisY);
    lineB->attachAxis(axisY);

    QChartView* chartView = new QChartView(chart);

    QHBoxLayout* serverInputs = new QHBoxLayout();
    serverInputs->addLayout(ipRecieveLayout);
    serverInputs->addLayout(ipSendLayout);
    serverInputs->addLayout(targetLayout);

    QVBoxLayout* serverButtons = new QVBoxLayout();
    serverButtons->addLayout(serverInputs);
    serverButtons->addWidget(btnSendTarget);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(chartView);
    mainLayout->addLayout(serverButtons);
    setLayout(mainLayout);

    connect(btnStart, &QPushButton::clicked, this, &QtApp::startServer);
    connect(btnStop, &QPushButton::clicked, this, &QtApp::stopServer);

    renderTimer = new QTimer(this);
    connect(renderTimer, &QTimer::timeout, this, &QtApp::updateUI);
    connect(btnSendTarget, &QPushButton::clicked, this, &QtApp::sendTargetValue);
}

QtApp::~QtApp() {
    stopServer();
    if (renderTimer) {
        renderTimer->stop();
    }
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

    if (bih) {
        bih->stop();
    }

    if (bihThread) {
        bihThread->quit();
        bihThread->wait();
    }
    WSACleanup();
}
void QtApp::startServer() {
    
    btnStart->setEnabled(false);
    btnStop->setEnabled(true);

    btnSendTarget->setEnabled(false);
    inputTarget->setEnabled(false);

    int recievePort = portRecieveInput->value();
    QString recieveIP = ipRecieveInput->text();

    series->clear();
    lineK->clear();
    lineB->clear();

    isFirstPoint = true;

    pointsBuffer.clear();
    kihBuffer.clear();
    bihBuffer.clear();

    secThread = new QThread(this);
    signaler = new UdpOn(nullptr, recievePort, recieveIP.toLocal8Bit().data());
    signaler->moveToThread(secThread);

    kihThread = new QThread(this);
    kih = new Kih();
    kih->moveToThread(kihThread);

    bihThread = new QThread(this);
    bih = new Bih();
    bih->moveToThread(bihThread);

    connect(secThread, &QThread::started, signaler, &UdpOn::run);
    connect(signaler, &UdpOn::logMessage, this, &QtApp::appendLog);
    connect(signaler, &UdpOn::dataParsed, this, &QtApp::handleData);

    connect(kihThread, &QThread::started, kih, &Kih::run);
    connect(this, &QtApp::toFilter, kih, &Kih::filter);
    connect(kih, &Kih::kihResult, this, &QtApp::kihFilterData);

    connect(bihThread, &QThread::started, bih, &Bih::run);
    connect(this, &QtApp::toFilter, bih, &Bih::filter);
    connect(bih, &Bih::bihResult, this, &QtApp::bihFilterData);
    
    connect(signaler, &UdpOn::finished, this, &QtApp::onWorkerFinished);

    secThread->start();
    kihThread->start();
    bihThread->start();

    renderTimer->start(50);
}

void QtApp::stopServer() {
    btnStop->setEnabled(false);
    btnSendTarget->setEnabled(true);
    inputTarget->setEnabled(true);
    renderTimer->stop();
    if (signaler) {
        signaler->stop();
    }
    if (kih) {
        kih->stop();
    }
    if (bih) {
        bih->stop();
    }
}

void QtApp::sendTargetValue() {
    float targetValue = static_cast<float>(inputTarget->value());
    int sendPort = portSendInput->value();
    QString sendIP = ipSendInput->text();
    SOCKET sendSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sendSock == INVALID_SOCKET) {
        appendLog("Error: Could not create socket for sending.");
        return;
    }

    struct sockaddr_in dest;
    ZeroMemory(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(sendPort);
    const char* targetIp = (sendIP.toLocal8Bit()).data();
    inet_pton(AF_INET, targetIp, &dest.sin_addr);

    char payload[sizeof(float)];
    std::memcpy(payload, &targetValue, sizeof(float));

    int bytesSent = sendto(sendSock, payload, sizeof(float), 0, (struct sockaddr*)&dest, sizeof(dest));

    if (bytesSent == SOCKET_ERROR) {
        appendLog(QString("Error sending target value. Code: %1").arg(WSAGetLastError()));
    }
    else {
        appendLog(QString("Successfully sent target value: %1").arg(static_cast<double>(targetValue)));
    }

    closesocket(sendSock);
}

void QtApp::onWorkerFinished() {
    if (signaler) {
        signaler->deleteLater();
        signaler = nullptr;
    }
    if (secThread) {
        secThread->quit();
        secThread->wait();
        secThread->deleteLater();
        secThread = nullptr;
    }
    if (kih) {
        kih = nullptr;
    }
    if (kihThread) {
        kihThread->quit();
        kihThread->wait();
        kihThread = nullptr;
    }
    if (bih) {
        bih = nullptr;
    }
    if (bihThread) {
        bihThread->quit();
        bihThread->wait();
        bihThread = nullptr;
    }

    btnStart->setEnabled(true); 
}

void QtApp::appendLog(const QString& msg) {
    //logConsole->append(msg);
}

void QtApp::handleData(uint32_t time, float value) {
    pointsBuffer.append(QPointF(time, value));
    emit toFilter(time, value);
    if (pointsBuffer.size() > 100) {
        pointsBuffer.removeFirst();
    }
    //axisX->setRange(series->at(0).x(), series->at(series->count() - 1).x());        axisY->setRange(0 - 10, 0+10);

}

void QtApp::kihFilterData(uint32_t time, float value) {
    kihBuffer.append(QPointF(time, value));
    if (kihBuffer.size() > 100) {
        kihBuffer.removeFirst();
    }
}

void QtApp::bihFilterData(uint32_t time, float value) {
    bihBuffer.append(QPointF(time, value));
    if (bihBuffer.size() > 100) {
        bihBuffer.removeFirst();
    }
}

void QtApp::updateUI() {
    if (pointsBuffer.isEmpty()) return;
    series->replace(pointsBuffer);
    lineK->replace(kihBuffer);
    lineB->replace(bihBuffer);

    qreal MinX = pointsBuffer.first().x();
    qreal MaxX = pointsBuffer.last().x();

    if (MinX == MaxX) MaxX = MinX + 1;

    axisX->setRange(MinX, MaxX);
    axisY->setRange(0 - 10, 0 + 10);
}



