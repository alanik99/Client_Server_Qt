#include "server.h"
#include <QThread>
#include <QDebug>
#include <QDataStream>
#include <QLocalSocket>
#include <QTime>
#include <QUuid>

Server::Server(QObject *parent) : QObject(parent)
{

}

Server::~Server()
{
    stopThreads();
}

bool Server::start()
{
    UdpServer* serv = new UdpServer();

    QThread* thread = new QThread;
    if (!thread)
        return false;

    serv->moveToThread(thread);


    connect(thread,SIGNAL(started()),serv,SLOT(process()));

    connect(serv,SIGNAL(finished()),thread,SLOT(quit()));

    connect(this,SIGNAL(stopAll()),serv,SLOT(stop()));

    connect(serv,SIGNAL(finished()),serv,SLOT(deleteLater()));

    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));

    connect(serv,SIGNAL(dataRecieved(QByteArray)),this,SLOT(dataRecieved(QByteArray)));


    thread->start();

    return true;
}

void Server::dataRecieved(QByteArray msg)
{
    qDebug()<< QTime::currentTime().toString("hh:mm:ss:zzz") << "dataRecieved";

    if (msg.size()>0)
    {
        qDebug()<< QTime::currentTime().toString("hh:mm:ss:zzz") << "Start handle";

        QDataStream in(&msg, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_0);

        qint64 size;
        in >> size;

        qint8 type;
        in >> type;

        qint64 counter_zap;
        in >> counter_zap;

        float h, y0, v0, tetta0, T, m0, cx0, cya, A, tr, mc, d;
        in >> h;
        in >> y0;
        in >> v0;
        in >> tetta0;
        in >> T;
        in >> m0;
        in >> cx0;
        in >> cya;
        in >> A;
        in >> tr;
        in >> mc;
        in >> d;

        QString msg;
        in >> msg;

        qDebug()<< QTime::currentTime().toString("hh:mm:ss:zzz");
        qDebug()<< " type = " << type
                << " size = " << size
                << " h = " << h
                << " y0 = " << y0
                << " v0 = " << v0
                << " tetta0 = " << tetta0
                << " T = " << T
                << " m0 = " << m0
                << " cx0 = " << cx0
                << " cya = " << cya
                << " A = " << A
                << " tr = " << tr
                << " mc = " << mc
                << " d = " << d
                << " message from server" << msg;

        WorkerData data(h, y0, v0, tetta0, T, m0, cx0, cya, A, tr, mc, d);
        addThread(data,QUuid::createUuid(),counter_zap);

        qDebug()<< QTime::currentTime().toString("hh:mm:ss:zzz") << "Finish handle";
    }
}

void Server::addThread(WorkerData data, QUuid id, qint64 counter)
{
    Worker* worker = new Worker(data, id, counter);
    QThread* thread = new QThread;
    worker->moveToThread(thread);

    connect(thread,SIGNAL(started()),worker,SLOT(process()));

    connect(worker,SIGNAL(finished()),thread,SLOT(quit()));

    connect(this,SIGNAL(stopAll()),worker,SLOT(stop()));

    connect(worker,SIGNAL(finished()),worker,SLOT(deleteLater()));

    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));

    thread->start();
}

void Server::stopThreads()
{
    emit stopAll();
}
