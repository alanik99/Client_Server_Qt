#include "worker.h"
#include <QDataStream>
#include <QDebug>
#include <QThread>
#include <unistd.h>
#include <QTime>

Worker::Worker(const WorkerData &data, const QUuid &id, qint64 counter)
{
    m_socket = nullptr;
    m_id = id;
    m_data = data;
    m_counter = counter;
}

void Worker::process()
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss:zzz") << m_id << "Worker process start.";

    if (m_socket == nullptr)
        m_socket = new QUdpSocket(this);

    QByteArray dataOut;
    QDataStream out(&dataOut, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);

    if (!m_data.isValid())
    {
        QString msg = "Server message. Data is not valid";
        out << qint64(0);
        out << qint8(2);
        out << msg;
        out.device()->seek(qint64(0));
        out << qint64(dataOut.size() - sizeof(qint64));

        qDebug()<< QTime::currentTime().toString("hh:mm:ss:zzz") << m_id << "send size" << dataOut.size();

        m_socket->writeDatagram(dataOut, QHostAddress::LocalHost,
                              36002);

        qDebug()<< QTime::currentTime().toString("hh:mm:ss:zzz") << "Invalid data";
        emit finished();
        return;
    }

    qDebug()<< QTime::currentTime().toString("hh:mm:ss:zzz") << m_id <<"Before calculate";
    Calculator * calc = new Calculator();
    qDebug() << QTime::currentTime().toString("hh:mm:ss:zzz") << m_id <<"Worker run calc";

    CalculationResult res = calc->calculate(m_data.d,m_data.h,m_data.v0,m_data.y0,m_data.cx0,m_data.A,
                           m_data.cya,m_data.tetta0,m_data.m0,m_data.mc,m_data.T,m_data.tr,1);

    qDebug()<< QTime::currentTime().toString("hh:mm:ss:zzz") << m_id <<"After calculate";

    QString msg = QVariant(m_counter).toString() +
            ") Server result:"
            "\nv = " + QVariant(res.v).toString() +
            ";\ntetta = " + QVariant(res.tetta).toString() +
            ";\ny = " + QVariant(res.y).toString() +
            ";\nx = " + QVariant(res.x).toString() + ";";

    out << qint64(0);
    out << qint8(2);
    out << msg;
    out.device()->seek(qint64(0));
    out << qint64(dataOut.size() - sizeof(qint64));

    m_socket->writeDatagram(dataOut, QHostAddress::LocalHost,
                          36002);

    qDebug()<< QTime::currentTime().toString("hh:mm:ss:zzz") << "Send data";
    emit finished();
}

void Worker::stop()
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss:zzz") << m_id << "Worker stop";
    delete m_socket;
    emit finished();
}
