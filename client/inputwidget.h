#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QVector>
#include <QUdpSocket>
#include "udpclient.h"

namespace Ui {
class inputWidget;
}

class inputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit inputWidget(QWidget *parent = nullptr);
     ~inputWidget();
    void create();
    void readvalues(QDoubleSpinBox);
    void setSettings(QDoubleSpinBox*);


signals:
    void stopAll();

public slots:
    void sendClick();
    void clearClick();
    void displayError();
    void dataRecieved(QString msg);

private:
    Ui::inputWidget *ui;
    QPushButton my_btn;
    QPushButton my_btn_clear;
    int minw = 200;
    int minh = 0;
    int decimals = 4;
    QVector<QDoubleSpinBox*> spinboxvec;
    QUdpSocket *m_socketWrite;
    QDataStream in;
    int counter_zap = 0;

};

#endif // INPUTWIDGET_H
