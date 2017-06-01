#include "transceiverthread.h"

TransceiverThread::TransceiverThread()
{
    resetState();
}

TransceiverThread::~TransceiverThread()
{

}

//设置为开始传输状态
void TransceiverThread::resetState()
{
    lineCounter = 0;
    ignoreFirstM01 = true;
}

//设置传输的数据与串口
void TransceiverThread::set(QString intextfile,SpherebotPort &uibot)
{
    lineCounter = 0;
    textfile.clear();
    textfile.append(removeComments(intextfile));
    //qDebug()<<"The textfile String is: \n\n" + textfile + "\n\nENDE\n\n";
    lineMax = textfile.count("\n");//包含某个特定字符串或字符的个数
    bot = &uibot;//得到串口引用
}

//线程运行
void TransceiverThread::run()
{
    qDebug()<<"entering run";
    lineCounter = 0;
    sendNext();
}

//循环发送数据
void TransceiverThread::sendNext()
{
    QString tmp;
    if(lineCounter < lineMax)
    {
        //字符串看成是几个块，这些块由 sep 分隔，start 和 end 指定块号
        tmp = textfile.section("\n",lineCounter,lineCounter);//得到一行数据
        if(tmp.contains("M01"))
        {
            if(ignoreFirstM01)
            {
                ignoreFirstM01 = false;
            }
            else
            {
                emit layerTransmitted();
            }
        }
        tmp.append("\n");
        bot->send(tmp);//通过串口发送一行数据，并更新数据发送窗口显示发送的数据
        double progress= (double) lineCounter/(double)lineMax;
        emit progressChanged(progress*100);//更新进度
        lineCounter++;//下一行
    }
    else
    {
        emit fileTransmitted();//数据发送完成
        resetState();
        return;
    }
    if(tmp.contains("G4"))//最后一条指令是G4，则线程休眠300毫秒
    {
        msleep(300);
    }
}

//得到当先发送到第几行数据
int TransceiverThread::getLineCounter()
{
    return lineCounter;
}

//删除空白与g-code的注释部分
QString removeComments(QString intext)
{
    ////////////////////////////////////////////////remove comments
    QString outTmp1,outTmp2;
    bool state=1;   //1= send, 0 = ignore
    for(int i=0;i<intext.size();i++)
    {
        if(intext.at(i) == '(' ) state=0;
        if(state == 1) outTmp1.append(intext[i]);
        if(intext.at(i) == ')' ) state=1;
    }
    //////////////////////////////////////////////////remove empty lines
    for(int i=0;i<outTmp1.size();i++)
    {
         if(i != outTmp1.size())
         {
              if(!(outTmp1[i] == '\n' && outTmp1[i+1] == '\n'))
              {
                  outTmp2.append(outTmp1[i]);
              }
          }
     }
     ///////////////////////////////////////////////////
     return outTmp2;
}
