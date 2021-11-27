#include "inputwidget.h"
#include "ui_inputwidget.h"
#include <QDebug>
#include <QString>
#include <QTime>
#include <unistd.h>
#include <QThread>

inputWidget::inputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::inputWidget),
    spinboxvec(12)
{
    ui->setupUi(this);

    UdpClient* client = new UdpClient();
    QThread* thread = new QThread;
    client->moveToThread(thread);

    connect(thread,SIGNAL(started()),client,SLOT(process()));

    connect(client,SIGNAL(finished()),thread,SLOT(quit()));

    connect(this,SIGNAL(stopAll()),client,SLOT(stop()));

    connect(client,SIGNAL(finished()),client,SLOT(deleteLater()));

    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));

    connect(client,SIGNAL(dataRecieved(QString)),this,SLOT(dataRecieved(QString)));

    thread->start();

    m_socketWrite = nullptr;
}

inputWidget::~inputWidget()
{
    delete ui;

    for(int i = 0; i < spinboxvec.size(); i++)
        delete spinboxvec[i];
}

void inputWidget::setSettings(QDoubleSpinBox *spinbox_ptr)
{
    spinbox_ptr->setMinimumSize(QSize(minw, minh));
    spinbox_ptr->setDecimals(decimals);
    spinbox_ptr->setMinimum(-10000);
    spinbox_ptr->setMaximum(10000);
    spinbox_ptr->setFont(QFont ("Courier New", 12));
    spinbox_ptr->setValue(1);
}

void inputWidget::create()
{
    qDebug() << QTime::currentTime().toString("hh:mm:ss:zzz") << "Create Client.";
    my_btn.setText("Отправить");
    my_btn.setFont(QFont ("Courier New", 11));
    ui->verticalLayout_2->addWidget(&my_btn);
    connect(&my_btn, SIGNAL(clicked()), this, SLOT(sendClick()));

    my_btn_clear.setText("Очистить");
    my_btn_clear.setFont(QFont ("Courier New", 11));
    ui->verticalLayout_5->addWidget(&my_btn_clear);
    connect(&my_btn_clear, SIGNAL(clicked()), this, SLOT(clearClick()));

    // Adding spinboxes
    for(int i = 0; i < spinboxvec.size(); i++)
        spinboxvec[i] = new QDoubleSpinBox;

    for(int i = 0; i < 6; i++)
    {
        ui->gridLayout->addWidget(spinboxvec.at(i), i, 1);
        setSettings(spinboxvec.at(i));
    }
    for(int i = 6; i < 12; i++)
    {
        ui->gridLayout_2->addWidget(spinboxvec.at(i), i-6, 1);
        setSettings(spinboxvec.at(i));
    }
}

void inputWidget::sendClick()
{
    counter_zap = counter_zap + 1;

    qDebug() << QTime::currentTime().toString("hh:mm:ss:zzz") << "Push SendButton.";
    qDebug() << "------------------------------------------------------------------------";
    float values[spinboxvec.length()];
    for(int i = 0; i < spinboxvec.length(); i++)
    {
        values[i] = spinboxvec.at(i)->value();
    }

    if (m_socketWrite == nullptr)
        m_socketWrite = new QUdpSocket(this);

    ui->textEdit->append(QString("-------------------------------------"));
    if (m_socketWrite){
        ui->textEdit->append(QString(QVariant(counter_zap).toString() + ") Client connected to socket."));
    } else{
        ui->textEdit->append(QString(QVariant(counter_zap).toString() + ") Client is not connected to socket."));
        return;
    }
    QString init = QString("Initial data:\n") +
            QString("h = ") + QVariant(values[0]).toString() + QString("\n") +
            QString("y0 = ") + QVariant(values[1]).toString() + QString("\n") +
            QString("v0 = ") + QVariant(values[2]).toString() + QString("\n") +
            QString("tetta0 = ") + QVariant(values[3]).toString() + QString("\n") +
            QString("T = ") + QVariant(values[4]).toString() + QString("\n") +
            QString("m0 = ") + QVariant(values[5]).toString() + QString("\n") +
            QString("cx0 = ") + QVariant(values[6]).toString() + QString("\n") +
            QString("cya = ") + QVariant(values[7]).toString() + QString("\n") +
            QString("A = ") + QVariant(values[8]).toString() + QString("\n") +
            QString("tr = ") + QVariant(values[9]).toString() + QString("\n") +
            QString("mc = ") + QVariant(values[10]).toString() + QString("\n") +
            QString("d = ") + QVariant(values[11]).toString();
    ui->textEdit->append(init);

    QByteArray dataOut;
    QDataStream out(&dataOut, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);

    out << qint64(0);
    out << qint8(2);
    out << qint64(counter_zap);

    for(int i = 0; i < spinboxvec.length(); i++)
        out << float(values[i]);

    QString msg = "Client message";
    out << msg;

    out.device()->seek(qint64(0));
    out << qint64(dataOut.size() - sizeof(qint64));

    m_socketWrite->writeDatagram(dataOut, QHostAddress::LocalHost,
                          36001);

    qDebug() << "------------------------------------------------------------------------";
}

void inputWidget::clearClick()
{
    ui->textEdit->clear();
    counter_zap = 0;
}

void inputWidget::displayError()
{
    qDebug() <<"ERROR";
}

void inputWidget::dataRecieved(QString msg)
{
    ui->textEdit->append(QString("-------------------------------------"));
    ui->textEdit->append(msg);
    ui->textEdit->ensureCursorVisible();
}
