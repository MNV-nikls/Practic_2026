#pragma once

#include <QWidget>
#include <QTextEdit>

#include <QThread>

#include <QPushButton>

#include <QTimer>
#include <QList>  
#include <QPointF>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSpinBox>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "udpOn.h"
#include "kih-alg.h"
#include "bih-alg.h"
#include <winsock2.h>


class QtApp : public QWidget {
    Q_OBJECT

public:
    explicit QtApp(QWidget* parent = nullptr);
    ~QtApp();

private slots:
    void startServer();
    void stopServer();
    void appendLog(const QString& msg);
    void handleData(uint32_t time, float value);
    void kihFilterData(uint32_t time, float value);
    void bihFilterData(uint32_t time, float value);
    void onWorkerFinished();
    void updateUI();
    void sendTargetValue();
    void applySettings();

signals:
    void toFilter(uint32_t time, float value);

private:
    QTextEdit* logConsole;
    QTextEdit* kihConsole;
    QTextEdit* bihConsole;

    QThread* secThread;
    QThread* kihThread;
    QThread* bihThread; 

    UdpOn* signaler;

    Kih* kih;
    Bih* bih;

    QPushButton* btnStart;
    QPushButton* btnStop;

    QLineEdit* ipRecieveInput;
    QSpinBox* portRecieveInput;
    QLineEdit* ipSendInput;
    QSpinBox* portSendInput;
    QDoubleSpinBox* inputTarget;
    QPushButton* btnSendTarget;
    QPushButton* btnSendIP;

    QChart* chart;
    QLineSeries* series;
    QLineSeries* lineK;
    QLineSeries* lineB;
    QLineSeries* ideal;
    QValueAxis* axisX;
    QValueAxis* axisY;

    bool isFirstPoint;
    QTimer* renderTimer;
    QList<QPointF> pointsBuffer;
    QList<QPointF> kihBuffer;
    QList<QPointF> bihBuffer;

    int SEND_PORT;
    QString SEND_IP;

};
