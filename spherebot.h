#ifndef SPHEREBOT_H
#define SPHEREBOT_H

#include <QMainWindow>
#include "spherebotport.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include "transceiverthread.h"
#include <QScrollBar>
#include <QTextCursor>
#include <QGraphicsSvgItem>
#include <QMessageBox>

#define DEFAULTDIAMETER 20 //默认直径
#define DEFAULTPENUP 40 //pen up
#define DEFAULTPENDOWN 50 //pen down
#define DEFAULTFEEDRATE 2000 //freedrate

#define MAXLISTITEMS 60 //显示发送数据的列表最大显示的条数

//在文件中查找参数
int getOption(QString string,QString searchString);

namespace Ui {
class SphereBot;
}

class SphereBot : public QMainWindow
{
    Q_OBJECT

    //设备状态枚举常量
    enum SendStates{Idle=0,Sending=1,Stoped=2};

public:
    explicit SphereBot(QWidget *parent = 0);
    ~SphereBot();
    void connectTranceiver();//串口连接传送对象
    void disconnectTranceiver();//串口与数据传输解除关联
    bool LoadSettings();//加载Settings表中的信息
    void SaveSettings();//保存信息到Settings表中
    void initUI();//初始化对话框
    void setState(SendStates state);//设置当前设备的状态
public slots:
    void receiveData();//接收串口传来的数据,显示在接收窗口中
    void sendDataUI(QString data);//串口发送出数据后，更新显示发送数据的界面
    void finishedTransmission();//传输数据完成时的操作
    void refreshSendProgress(int value);//刷新发送数据的进度
    void finishedLayer();//图层改变时的操作
    void fitgraphicsView();//显示svg

private slots:
    void on_connectButton_clicked();//串口如果打开则断开,如果断开者打开
    void resetPortList();//刷新可用串口并显示
    void on_sendButton_clicked();//单行文本框发送指令数据
    void on_resetButton_clicked();//刷新串口信息
    void on_sendString_editingFinished();//单行文本框发送指令数据
    void on_servoSlider_sliderMoved(int position);//滑动条调节舵机角度
    void on_servospinBox_valueChanged(int arg1);//调节框调节舵机角度
    void on_penSlider_valueChanged(int value);//滑动条调节Y轴位置
    void on_eggSlider_valueChanged(int value);//滑动条调节X轴位置
    void on_penRotationBox_valueChanged(int arg1);//调节框调节Y轴位置
    void on_eggRotationBox_valueChanged(int arg1);//调节框调节X轴位置
    void on_loadFileButton_clicked();//加载文件到文本框
    void on_saveFileButton_clicked();//保存文件
    void on_fileTextEdit_textChanged();//加载进来文件
    void on_sendFileButton_clicked();//点击发送文件按钮如果是空闲状态则发送数据，如果是发送状态则停止，如果是停止则继续发送
    void on_servoFeedrateSlider_valueChanged(int value);//改变设备速度
    void on_setDiameterButton_clicked();//调节球状物直径
    void on_undoButton_clicked();//撤销
    void on_redoButton_clicked();//重做
    void on_fileTextEdit_undoAvailable(bool b);//文本可以撤销时的处理
    void on_fileTextEdit_redoAvailable(bool b);//文本可以重做时的处理
    void on_restartButton_clicked();//从新发送数据文件

    void interpretSentString(QString string);//根据串口发送的数据更新控制电机的实时更新点击的界面

    void on_sendString_textChanged(const QString &arg1);//发送单行文本框内容改变时的处理

private:

    void extractOptions(QString file); //得到文件中的参数
    void loadFile(const QString &fileName);//根据文件名加载文件
    bool saveFile(const QString &fileName);//保存文件
    void interpretGcode(QString code);//for future gcode interpretation
    void refreshLayerNames(QString file);//通过M01得到每个图层的名字
    void SetBotToHomePosition();//发送Y方向的直线运动到0并抬笔命令
    QString curFile;//当前文件
    QString curDir;//包含文件名的绝对路径
    SendStates sendState;//当前状态

    Ui::SphereBot *ui;

    QSerialPortInfo PortInfo;//串口信息
    QList<QSerialPortInfo> portList;//串口信息列表
    SpherebotPort *bot;//窗口操作类
    TransceiverThread transceiver;//传输数据
    QGraphicsScene *scene;//图像视图场景
    QList<QString> layerNames;   //每个图层的名字     //layerFile, layerColorString
    int layerIndex;//当前图层索引

    QMessageBox *nextLayerMsgBox;//下一图层提示对话框
    QMessageBox *restartLayerMsgBox;//从新发送数据对话框

    int penUpAngle;//抬笔角度
    int penDownAngle;//落笔角度

    QTimer FitInTimer;          //timer to trigger the fitIn function for the graphics view. Actually this shouldn´t be necessary!
    bool isCon;//信号与槽是否连接
    bool isSendButton;//是否是sendButton发送数据
};

#endif // SPHEREBOT_H
