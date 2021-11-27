#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QUuid>
#include <calculator.h>

struct WorkerData
{
    WorkerData(float h,float y0, float v0, float tetta0,
                float T, float m0, float cx0, float cya,
                float A, float tr, float mc, float d) :

        h(h), y0(y0), v0(v0), tetta0(tetta0),
        T(T), m0(m0), cx0(cx0), cya(cya),
        A(A), tr(tr), mc(mc), d(d)
    {
        m_validate = true;
    }

    WorkerData() {}

    float h;
    float y0;
    float v0;
    float tetta0;
    float T;
    float m0;
    float cx0;
    float cya;
    float A;
    float tr;
    float mc;
    float d;

    bool isValid() const { return m_validate; }

private:
    bool m_validate = false;
};

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(const WorkerData & data, const QUuid & id, qint64 counter);

public slots:
    void process();
    void stop();

signals:
    void finished();

private:
    QUdpSocket *m_socket;
    QUuid m_id;
    WorkerData m_data;
    qint64 m_counter;
};

#endif // WORKER_H
