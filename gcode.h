#ifndef GCODE_H
#define GCODE_H

#include<QObject>
#include<QList>
#include<QString>
#include<QPointF>
#include<QFile>
#include<QTextStream>
#include<QMessageBox>
#include"datadef.h"


class Gcode
{
public:
    Gcode();
    Gcode(float upAngle,float downAngle,float delayDown,float delayUp,float xyfeedrate,float zfeedrate=Z_FEEDRATE,float zheight=Z_HEIGHT,float zafterheight=Z_AFTER_HEIGHT,float x=X_START_VALUE,float y=Y_START_VALUE);
    ~Gcode();

    void addGcodeHead(QString file);//增加gcode文件头
    void addPostScript();//增加gcode尾部
    void addStart();//增加一条线的开始
    void addStop();//增加一条线的结束
    void goToPoint(float x,float y,bool stop=false);//去某一点
    void drawToPoint(float x,float y,bool stoop=false);//运动到某一点去
    void addWidth(float width);//增加线宽
    void addNote(QString note);//添加注释

    bool printFile(const QString &fileName);//输出为文件

private:
    QList<QString> gcodeList;//gcode文件数据
    float penUpAngle;//抬笔高度
    float penDownAngle;//落笔高度
    float delayAfterPenDown;//落笔后延迟时间
    float delayAfterPenUp;//抬笔后延迟时间
    float xyFeedrate;//xy方向旋转速度
    float zFeedrate;//z轴速度
    float zHeight;//z轴初始高度
    float zAfterHeight;//z轴打印结束时的高度
    float xStartValue;//笔的开始位置x，这个位置取决于物体形状的轴线方向
    float yStartValue;//笔的开始位置y，这个位置取决于物体形状的轴线方向

    bool drawing;//是否在画图
    QPointF lastPoint;//上一个点
};

#endif // GCODE_H
