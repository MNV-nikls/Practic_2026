#include <QApplication>
#include "Qtapp.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QtApp window;
    window.show();

    return app.exec();
}
