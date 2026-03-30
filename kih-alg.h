#pragma once

#include <QObject>
#include <QString>

#define STEPS 3

class Kih : public QObject {
    Q_OBJECT

public:
    explicit Kih(QObject* parent = nullptr);
    ~Kih();

    void stop();

public slots:
    void run();
    void filter(uint32_t time, float value);

signals:
    void kihResult(uint32_t time, float value);
    void kihMessage(const QString& msg);
    void kihClose();

private:
    std::atomic<bool> status;
    float* coef = new float[STEPS];
    float* point = new float[STEPS];
};