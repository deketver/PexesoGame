#include "setter.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setter w;
    w.show();
    QObject::connect(&w, SIGNAL(readyToQuit()), &a, SLOT(quit()));
    return a.exec();
}
