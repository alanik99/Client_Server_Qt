#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include "worker.h"
#include "udpserver.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
    bool start();

signals:
    void stopAll();

public slots:
    void dataRecieved(QByteArray msg);

private:
    void addThread(WorkerData data, QUuid id, qint64 counter);
    void stopThreads();
};

#endif // SERVER_H
