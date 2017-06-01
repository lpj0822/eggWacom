#include "gcode.h"

Gcode::Gcode():lastPoint(1340.0,361.0)
{
    this->penUpAngle=PEN_UP_ANGLE;
    this->penDownAngle=PEN_DOWN_ANGLE;
    this->delayAfterPenDown=DELAY_PEN_DOWN;
    this->delayAfterPenUp=DELAY_PEN_UP;
    this->xyFeedrate=XY_FEEDRATE;
    this->zFeedrate=Z_FEEDRATE;
    this->zHeight=Z_HEIGHT;
    this->zAfterHeight=Z_AFTER_HEIGHT;
    this->xStartValue=X_START_VALUE;
    this->yStartValue=Y_START_VALUE;

    this->drawing=false;//是否在画图
    this->gcodeList.clear();
}

Gcode::Gcode(float upAngle,float downAngle,float delayDown,float delayUp,float xyfeedrate,float zfeedrate,float zheight,float zafterheight,float x,float y)
    :lastPoint(1340.0,361.0)
{
    this->penUpAngle=upAngle;
    this->penDownAngle=downAngle;
    this->delayAfterPenDown=delayDown;
    this->delayAfterPenUp=delayUp;
    this->xyFeedrate=xyfeedrate;
    this->zFeedrate=zfeedrate;
    this->zHeight=zheight;
    this->zAfterHeight=zafterheight;
    this->xStartValue=x;
    this->yStartValue=y;

    this->drawing=false;
    this->gcodeList.clear();
}

Gcode::~Gcode()
{

}

//增加gcode文件头
void Gcode::addGcodeHead(QString file)
{
    QString str1;
    QString str2;
    QString str3;
    int upangle=penUpAngle;
    int downangle=penDownAngle;
    int delaydown=delayAfterPenDown;
    int delayup=delayAfterPenUp;
    int xyfeedrate=xyFeedrate;
    int zfeedrate=zFeedrate;
    int zheight=zHeight;
    int zafterheight=zAfterHeight;
    int x=xStartValue;
    int y=yStartValue;
    str1.sprintf("(Scribbled version of %s @ %.2f)",file.toUtf8().data(),xyFeedrate);
    str2.sprintf("( --pen-up-angle=%d --pen-down-angle=%d --start-delay=%d --stop-delay=%d --xy-feedrate=%d --z-feedrate=%d --z-height=%d --finished-height=%d --register-pen=false --x-home=%d --y-home=%d --num-copies=1 --continuous=false --pause-on-layer-change=false )",
                 upangle,downangle,delaydown,delayup,xyfeedrate,zfeedrate,zheight,zafterheight,x,y);
    str3.sprintf("G92 X%.2f Y%.2f Z%.2f (you are here)",xStartValue,yStartValue,zHeight);
    gcodeList.append(str1);
    gcodeList.append(str2);
    gcodeList.append(QObject::tr("G21 (metric ftw)"));
    gcodeList.append(QObject::tr("G90 (absolute mode)"));
    gcodeList.append(str3);
    gcodeList.append(QObject::tr(""));
}

//增加gcode尾部
void Gcode::addPostScript()
{
    QString str1;
    QString str2;
    QString str3;
    QString str4;
    QString str5;
    int delayUp=delayAfterPenUp;
    str1.sprintf("M300 S%.2f (pen up)",penUpAngle);
    str2.sprintf("G4 P%d (wait %dms)",delayUp,delayUp);
    str3.sprintf("G1 X0 Y0 F%.2f",xyFeedrate);
    str4.sprintf("G1 Z%.2f F%.2f (go up to finished level)",zAfterHeight,zFeedrate);
    str5.sprintf("G1 X%.2f Y%.2f F%.2f (go home)",xStartValue,yStartValue,xyFeedrate);
    gcodeList.append(QObject::tr(""));
    gcodeList.append(QObject::tr("(end of print job)"));
    gcodeList.append(str1);
    gcodeList.append(str2);
    gcodeList.append(QObject::tr("M300 S255 (turn off servo)"));
    gcodeList.append(str3);
    gcodeList.append(str4);
    gcodeList.append(str5);
    gcodeList.append(QObject::tr("M18 (drives off)"));
}

//增加一条线的开始
void Gcode::addStart()
{
    QString str1;
    QString str2;
    int delaydown=delayAfterPenDown;
    str1.sprintf("M300 S%.2f (pen down)",penDownAngle);
    str2.sprintf("G4 P%d (wait %dms)",delaydown,delaydown);
    gcodeList.append(str1);
    gcodeList.append(str2);
    drawing=true;
}

//增加一条线的结束
void Gcode::addStop()
{
    QString str1;
    QString str2;
    int delayup=delayAfterPenUp;
    str1.sprintf("M300 S%.2f (pen up)",penUpAngle);
    str2.sprintf("G4 P%d (wait %dms)",delayup,delayup);
    gcodeList.append(str1);
    gcodeList.append(str2);
    drawing=false;
}

//去某一点
void Gcode::goToPoint(float x,float y,bool stop)
{
    QString str1;
    QString str2;
    QString str3;
    int delayup=delayAfterPenUp;
    str1.sprintf("M300 S%.2f (pen up)",penUpAngle);
    str2.sprintf("G4 P%d (wait %dms)",delayup,delayup);
    str3.sprintf("G1 X%.2f Y%.2f F%.2f",x,y,xyFeedrate);
    if(lastPoint.x()==x&&lastPoint.y()==y)
    {
        return;
    }
    if(stop)
    {
        return;
    }
    else
    {
        if(drawing)
        {
            gcodeList.append(str1);
            gcodeList.append(str2);
            drawing=false;
        }
        gcodeList.append(str3);
    }
    lastPoint.setX(x);
    lastPoint.setY(y);
}

//运动到某一点去
void Gcode::drawToPoint(float x,float y,bool stop)
{
    QString str1;
    QString str2;
    QString str3;
    int delaydown=delayAfterPenDown;
    str1.sprintf("M300 S%.2f (pen down)",penDownAngle);
    str2.sprintf("G4 P%d (wait %dms)",delaydown,delaydown);
    str3.sprintf("G1 X%.2f Y%.2f F%.2f",x,y,xyFeedrate);
    if(lastPoint.x()==x&&lastPoint.y()==y)
    {
        return;
    }
    if(stop)
    {
        return;
    }
    else
    {
        if(drawing==false)
        {
            gcodeList.append(str1);
            gcodeList.append(str2);
            drawing=true;
        }
        gcodeList.append(str3);
    }
    lastPoint.setX(x);
    lastPoint.setY(y);
}

//增加线宽
void Gcode::addWidth(float width)
{
    QString str;
    str.sprintf("M301 S%.2f (pen width)",width);
    gcodeList.append(str);
}

//添加注释
void Gcode::addNote(QString note)
{
    gcodeList.append(note);
}

//输出为文件
bool Gcode::printFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(NULL, QObject::tr("保存gcode"),
                             QObject::tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    for(int loop=0;loop<gcodeList.count();loop++)
    {
        out<<gcodeList[loop]<<endl;
    }
    return true;
}


