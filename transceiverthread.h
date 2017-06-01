#ifndef TRANSCEIVERTHREAD_H
#define TRANSCEIVERTHREAD_H

#include "spherebotport.h"
#include <QMessageBox>
#include <QTimer>
#include <QString>

//删除空白与g-code的注释部分
QString removeComments(QString intext);

class TransceiverThread : public QThread
{
    Q_OBJECT
public:
    TransceiverThread();
    ~TransceiverThread();

    void set(QString textfile,SpherebotPort &bot);//设置传输的数据与串口
    void run();//线程运行
    int getLineCounter();//得到当先发送到第几行数据

signals:
    void progressChanged(int);//更新进度条消息
    void layerTransmitted();//图层变换消息
    void fileTransmitted();//数据发送完成

public slots:
    void sendNext();//循环发送数据
    void resetState();//设置传输开始的状态
private:
    QString textfile;//最终传输的数据
    int lineCounter;//执行到第一行
    int lineMax;//数据一共有多少行
    bool ignoreFirstM01;//图层变换
    SpherebotPort *bot;//串口操作类
};

#endif // TRANSCEIVERTHREAD_H
