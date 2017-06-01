#ifndef SPHEREBOTPORT_H
#define SPHEREBOTPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QtGui>
#include <iostream>
#include <QtCore/QCoreApplication>

class SpherebotPort : public QObject
{
    Q_OBJECT
public:
    QSerialPort *port;

    bool isConnected();//串口是否打开
    bool send(QString cmd);//通过串口发送数据并更新发送数据窗口
    //explicit主要用于 "修饰 "构造函数. 指明构造函数只能显示使用,目的是为了防止不必要的隐式转化.
    explicit SpherebotPort(QObject *parent = 0);
    ~SpherebotPort();

signals:
    void dataSent(QString data);//串口发送数据后的信号

public slots:
    bool connectWithBot(QString portName);//根据串口名打开串口
    bool connectWithBot();//打开串口并设置参数
    bool disconnectWithBot();//关闭串口

private:
    bool port_connected;//串口是否打开
};

#endif // SPHEREBOTPORT_H
