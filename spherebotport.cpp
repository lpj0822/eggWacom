#include "spherebotport.h"

SpherebotPort::SpherebotPort(QObject *parent) : QObject(parent)
{
    port = new QSerialPort();
    port_connected = false;
}

SpherebotPort::~SpherebotPort()
{

}

//打开串口并设置参数
bool SpherebotPort::connectWithBot()
{
    port->open(QIODevice::ReadWrite); //we open the port
    if(!port->isOpen())
    {
        //QMessageBox::warning(this, "port error", "Impossible to open the port!");
        return 0;
    }

    //we set the port properties
    //波特率
    port->setBaudRate(115200);//modify the port settings on your own
    port->setFlowControl(QSerialPort::NoFlowControl);
    //奇偶校验
    port->setParity(QSerialPort::NoParity);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port_connected = true;
    return 1;
}

//根据串口名打开串口
bool SpherebotPort::connectWithBot(QString portName)
{
    port->setPortName(portName);
    return connectWithBot();
}

//关闭串口
bool SpherebotPort::disconnectWithBot()
{
    qDebug("port closed");
    port->close();
    port_connected = false;
    return 1;
}

//通过串口发送数据并更新发送数据窗口
bool SpherebotPort::send(QString cmd)
{
    if(port->isOpen())
    {
        port->flush();
        if(cmd[cmd.size()-1] != '\n')
            cmd.append("\n");
        //qDebug()<<"Sending: " + cmd.toUtf8();
        port->write((const char*)cmd.toUtf8(),cmd.length());
    }
    else
    {
        qDebug()<<port->errorString()<<" port is not open";
        return false;
    }
    cmd.chop(1);//截掉最后的1个字符
    emit dataSent(cmd);
    return true;
}

//串口是否打开
bool SpherebotPort::isConnected()
{
    return port_connected;
}
