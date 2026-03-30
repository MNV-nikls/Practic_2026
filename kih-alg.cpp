#include "kih-alg.h"
#include <ws2tcpip.h>
#include <cstring>

#define STEPS 3

Kih::Kih(QObject* parent) : QObject(parent), status(false){}

Kih::~Kih() {
    stop();
}

void Kih::stop() {
    status = false;
    delete[] coef;
    delete[] point;
}

void Kih::run() {
    status = true;
    emit kihMessage("Start KIH-Filter");
    coef[0] = 0.5;
    coef[1] = 0.3;
    coef[2] = 0.6;
}

void Kih::filter(uint32_t time, float value) {
    for (int i = 0; i < (STEPS - 1); i++) {
        point[i] = point[i+1];
    }
    point[STEPS - 1] = value;
    float Res = 0;
    for (int i = 0; i < STEPS; i++) {
        Res += point[i]*coef[i];
    }
    emit kihResult(time, Res);
}
