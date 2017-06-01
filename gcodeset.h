#ifndef GCODESET_H
#define GCODESET_H

#include<QObject>
#include<QDialog>
#include<QDoubleSpinBox>
#include<QTabWidget>
#include<QVBoxLayout>
#include<QGridLayout>
#include<QDialogButtonBox>
#include<QPushButton>
#include<QLabel>
#include"gcodesetdata.h"
#include"datadef.h"

//初始化设置面板
class SetupWidget: public QWidget
{
    Q_OBJECT

public:
     SetupWidget(QWidget *parent = 0);
     SetupWidget(float upAngle,float downAngle,float delayDown,float delayUp,float xyFeedrate,float zFeedrate=Z_FEEDRATE,float zHeight=Z_HEIGHT,float zAfterHeight=Z_AFTER_HEIGHT,QWidget *parent=0);

     void initGUI();//初始化

     QDoubleSpinBox *upPenSpinBox;//抬笔高度
     QDoubleSpinBox *downPenSpinBox;//落笔高度
     QDoubleSpinBox *delayDownPenSpinBox;//落笔后延迟时间
     QDoubleSpinBox *delayUpPenSpinBox;//抬笔后延迟时间
     QDoubleSpinBox *xyFeedrateSpinBox;//xy方向旋转速度
     QDoubleSpinBox *zFeedrateSpinBox;//z轴速度
     QDoubleSpinBox *zHeightSpinBox;//z轴初始高度
     QDoubleSpinBox *zAfterHeightSpinBox;//z轴打印结束时的高度

private:
     QGridLayout *mainlayout;

     QLabel *upPenLabel;
     QLabel *downPenLabel;
     QLabel *delayDownPenLabel;
     QLabel *delayUpPenLabel;
     QLabel *xyFeedrateLabel;
     QLabel *zFeedrateLabel;
     QLabel *zHeightLabel;
     QLabel *zAfterHeightLabel;
};

//home面板
class HomeWidget: public QWidget
{
    Q_OBJECT

public:
     HomeWidget(float x=X_START_VALUE,float y=Y_START_VALUE,QWidget *parent = 0);

     void initGUI();//初始化

     QDoubleSpinBox *xSpinBox;//笔的开始位置x，这个位置取决于物体形状的轴线方向
     QDoubleSpinBox *ySpinBox;//笔的开始位置y，这个位置取决于物体形状的轴线方向

private:
     QGridLayout *mainlayout;
     QLabel *label;
     QLabel *xLabel;
     QLabel *yLabel;
};

//help面板
class HelpWidget: public QWidget
{
    Q_OBJECT

public:
     HelpWidget(QWidget *parent = 0);

private:
     QLabel *helpLabel;
};

class GcodeSet : public QDialog
{
    Q_OBJECT

public:
    GcodeSet(QWidget *parent = 0,float upAngle=PEN_UP_ANGLE,float downAngle=PEN_DOWN_ANGLE,float delayDown=DELAY_PEN_DOWN,float delayUp=DELAY_PEN_UP,float xyfeedrate=XY_FEEDRATE,float zfeedrate=Z_FEEDRATE,float zheight=Z_HEIGHT,float zafterheight=Z_AFTER_HEIGHT,float x=X_START_VALUE,float y=Y_START_VALUE);

    GcodeSetData getData();

signals:

public slots:
    void okSlot();//点击确定

private:
    QVBoxLayout *mainLayout;//布局
    QTabWidget *tabweight;//tab
    QDialogButtonBox *buttonBox;
    SetupWidget *setupwidget;//setup面板
    HomeWidget *homewidget;//home面板
    HelpWidget *helpwiget;//help面板

    GcodeSetData data;//gcode设置参数的数据
};

#endif // GCODESET_H
