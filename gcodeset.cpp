#pragma execution_character_set("utf-8")
#include "gcodeset.h"

//setup面板
SetupWidget::SetupWidget(QWidget *parent):QWidget(parent)
{
  initGUI();
  upPenSpinBox->setValue(PEN_UP_ANGLE);
  downPenSpinBox->setValue(PEN_DOWN_ANGLE);
  delayDownPenSpinBox->setValue(DELAY_PEN_DOWN);
  delayUpPenSpinBox->setValue(DELAY_PEN_UP);
  xyFeedrateSpinBox->setValue(XY_FEEDRATE);
  zFeedrateSpinBox->setValue(Z_FEEDRATE);
  zHeightSpinBox->setValue(Z_HEIGHT);
  zAfterHeightSpinBox->setValue(Z_AFTER_HEIGHT);
}

SetupWidget::SetupWidget(float upAngle,float downAngle,float delayDown,float delayUp,float xyFeedrate,float zFeedrate,float zHeight,float zAfterHeight,QWidget *parent)
    :QWidget(parent)
{
    initGUI();
    upPenSpinBox->setValue(upAngle);
    downPenSpinBox->setValue(downAngle);
    delayDownPenSpinBox->setValue(delayDown);
    delayUpPenSpinBox->setValue(delayUp);
    xyFeedrateSpinBox->setValue(xyFeedrate);
    zFeedrateSpinBox->setValue(zFeedrate);
    zHeightSpinBox->setValue(zHeight);
    zAfterHeightSpinBox->setValue(zAfterHeight);
}

//初始化
void SetupWidget::initGUI()
{
    upPenLabel=new QLabel(tr("抬笔角度："));
    upPenSpinBox=new QDoubleSpinBox();
    upPenSpinBox->setRange(0.0, 180.0);
    upPenSpinBox->setSingleStep(1.0);

    downPenLabel=new QLabel(tr("落笔角度："));
    downPenSpinBox=new QDoubleSpinBox();
    downPenSpinBox->setRange(0.0, 180.0);
    downPenSpinBox->setSingleStep(1.0);

    delayDownPenLabel=new QLabel(tr("落笔后延迟时间："));
    delayDownPenSpinBox=new QDoubleSpinBox();
    delayDownPenSpinBox->setRange(0.0, 2000.0);
    delayDownPenSpinBox->setSingleStep(1.0);

    delayUpPenLabel=new QLabel(tr("抬笔后延迟时间："));
    delayUpPenSpinBox=new QDoubleSpinBox();
    delayUpPenSpinBox->setRange(0.0, 2000.0);
    delayUpPenSpinBox->setSingleStep(1.0);

    xyFeedrateLabel=new QLabel(tr("xy方向旋转速度："));
    xyFeedrateSpinBox=new QDoubleSpinBox();
    xyFeedrateSpinBox->setRange(0.0, 10000.0);
    xyFeedrateSpinBox->setSingleStep(1.0);

    zFeedrateLabel=new QLabel(tr("z轴速度："));
    zFeedrateSpinBox=new QDoubleSpinBox();
    zFeedrateSpinBox->setRange(0.0, 1000.0);
    zFeedrateSpinBox->setSingleStep(1.0);

    zHeightLabel=new QLabel(tr("z轴初始高度："));
    zHeightSpinBox=new QDoubleSpinBox();
    zHeightSpinBox->setRange(-1000.0, 1000.0);
    zHeightSpinBox->setSingleStep(1.0);

    zAfterHeightLabel=new QLabel(tr("z轴打印结束时的高度："));
    zAfterHeightSpinBox=new QDoubleSpinBox();
    zAfterHeightSpinBox->setRange(-1000.0, 1000.0);
    zAfterHeightSpinBox->setSingleStep(1.0);

    mainlayout=new QGridLayout();
    mainlayout->addWidget(upPenLabel,0,0);
    mainlayout->addWidget(upPenSpinBox,0,1);

    mainlayout->addWidget(downPenLabel,1,0);
    mainlayout->addWidget(downPenSpinBox,1,1);

    mainlayout->addWidget(delayDownPenLabel,2,0);
    mainlayout->addWidget(delayDownPenSpinBox,2,1);

    mainlayout->addWidget(delayUpPenLabel,3,0);
    mainlayout->addWidget(delayUpPenSpinBox,3,1);

    mainlayout->addWidget(xyFeedrateLabel,4,0);
    mainlayout->addWidget(xyFeedrateSpinBox,4,1);

    mainlayout->addWidget(zFeedrateLabel,5,0);
    mainlayout->addWidget(zFeedrateSpinBox,5,1);

    mainlayout->addWidget(zHeightLabel,6,0);
    mainlayout->addWidget(zHeightSpinBox,6,1);

    mainlayout->addWidget(zAfterHeightLabel,7,0);
    mainlayout->addWidget(zAfterHeightSpinBox,7,1);

    setLayout(mainlayout);
}

//home面板
HomeWidget::HomeWidget(float x,float y,QWidget *parent):QWidget(parent)
{
    initGUI();
    xSpinBox->setValue(x);
    ySpinBox->setValue(y);
}

void HomeWidget::initGUI()
{
    label=new QLabel(tr("您可以设置笔的开始位置：\n笔的开始位置x，这个位置取决于物体形状的轴线方向\n笔的开始位置y，这个位置取决于物体形状的轴线方向"));
    xLabel=new QLabel(tr("笔的开始位置x："));
    xSpinBox=new QDoubleSpinBox();
    xSpinBox->setRange(-1000.0, 1000.0);
    xSpinBox->setSingleStep(1.0);

    yLabel=new QLabel(tr("笔的开始位置y："));
    ySpinBox=new QDoubleSpinBox();
    ySpinBox->setRange(-1000.0, 1000.0);
    ySpinBox->setSingleStep(1.0);

    mainlayout=new QGridLayout();
    mainlayout->addWidget(label,0,0,3,2);
    mainlayout->addWidget(xLabel,1,0);
    mainlayout->addWidget(xSpinBox,1,1);
    mainlayout->addWidget(yLabel,2,0);
    mainlayout->addWidget(ySpinBox,2,1);
    setLayout(mainlayout);
}

//help面板
HelpWidget::HelpWidget(QWidget *parent):QWidget(parent)
{
    QVBoxLayout *mainlayout=new QVBoxLayout();
    helpLabel=new QLabel(tr("GCode代码输出\n初始参数设置"));
    mainlayout->addWidget(helpLabel);
    setLayout(mainlayout);
}

//GCode设置参数设置面板
GcodeSet::GcodeSet(QWidget *parent,float upAngle,float downAngle,float delayDown,float delayUp,float xyfeedrate,float zfeedrate,float zheight,float zafterheight,float x,float y)
    :QDialog(parent),data(upAngle,downAngle,delayDown,delayUp,xyfeedrate,zfeedrate,zheight,zafterheight,x,y)
{
    tabweight=new QTabWidget();
    setupwidget=new SetupWidget(upAngle,downAngle,delayDown,delayUp,xyfeedrate,zfeedrate,zheight,zafterheight);
    homewidget=new HomeWidget(x,y);
    helpwiget=new HelpWidget();

    tabweight->addTab(setupwidget,tr("Setup"));
    tabweight->addTab(homewidget,tr("Home"));
    tabweight->addTab(helpwiget,tr("Help"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel); // 创建QDialogButtonBox
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));//将buttonbox中的ok 变成汉化
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(okSlot()));  // 为button定义连接信号槽
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    mainLayout=new QVBoxLayout();
    mainLayout->addWidget(tabweight);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
    setMinimumSize(400,400);
    setMaximumSize(400,400);
    setWindowTitle(tr("生成Gcode代码参数设置"));
}

//返回设置的数据
GcodeSetData GcodeSet::getData()
{
    return data;
}

//确定
void GcodeSet::okSlot()
{
    float upangle=setupwidget->upPenSpinBox->value();
    float downangle=setupwidget->downPenSpinBox->value();
    float delaydown=setupwidget->delayDownPenSpinBox->value();
    float delayup=setupwidget->delayUpPenSpinBox->value();
    float xyfeedrate=setupwidget->xyFeedrateSpinBox->value();
    float zfeedrate=setupwidget->zFeedrateSpinBox->value();
    float zheight=setupwidget->zHeightSpinBox->value();
    float zafterheight=setupwidget->zAfterHeightSpinBox->value();
    float x=homewidget->xSpinBox->value();
    float y=homewidget->ySpinBox->value();
    data.setPenUpAngle(upangle);
    data.setPenDownAngle(downangle);
    data.setDelayAfterPenDown(delaydown);
    data.setDelayAfterPenUp(delayup);
    data.setXYFeedrate(xyfeedrate);
    data.setZFeedrate(zfeedrate);
    data.setZHeight(zheight);
    data.setZAfterHeight(zafterheight);
    data.setXStartValue(x);
    data.setYStartValue(y);
    accept();
}

