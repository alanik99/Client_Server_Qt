#include <QCoreApplication>
#include <QDebug>
#include "server.h"
#include <QTime>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server ser;

    if (ser.start())
    {
        qDebug() << QTime::currentTime().toString("hh:mm:ss:zzz") <<"Server started.";
        return a.exec();
    }
    else
    {
        qDebug() << "Failed to start server.";
        return -1;
    }
}
