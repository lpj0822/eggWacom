#pragma execution_character_set("utf-8")
#include "wacomcanvas.h"
#include <QtWidgets>
#include <QMessageBox>
#include <QDebug>
#include <math.h>

WacomCanvas::WacomCanvas(QWidget *parent) : QWidget(parent)
{
    resize(WINDOWWIDTH, WINDOWHEIGHT);
    initData();
    initPixmap();
    setAutoFillBackground(true);
    setCursor(Qt::CrossCursor);
}

//数据初始化
void WacomCanvas::initData()
{
    myBrush = QBrush();
    myPen = QPen();
    myPen.setCapStyle(Qt::RoundCap);
    myPen.setJoinStyle(Qt::RoundJoin);

    deviceDown = false;
    myColor = Qt::red;
    myTabletDevice = QTabletEvent::Stylus;
    alphaChannelType = NoAlpha;
    colorSaturationType = NoSaturation;
    lineWidthType = LineWidthPressure;

    widthWeight=WIDTHWEIGHT;//宽度权值

    imageWidth=WINDOWWIDTH;//图片宽度
    imageHeight=WINDOWHEIGHT;//图片宽度
}

void WacomCanvas::initPixmap()
{
    QPixmap newPixmap = QPixmap(imageWidth,imageHeight);
    newPixmap.fill(Qt::white);
    QPainter painter(&newPixmap);
    if (!pixmap.isNull())
    {
        painter.drawPixmap(0, 0, pixmap);
    }
    painter.end();
    pixmap = newPixmap;

    //pixmap=QPixmap(imageWidth,imageHeight);
    //pixmap.fill(Qt::white);
    for(auto shape1 : myShapes)
    {
        if(shape1!=NULL)
            delete shape1;
    }
    myShapes.clear();
    for(auto shape2 : reShapes)
    {
        if(shape2!=NULL)
            delete shape2;
    }
    reShapes.clear();

    preWidth=0.0;//前一条线的宽度
    curWidth=0.0;//当前宽度
    listPoint.clear();//当前画的直线
    listWidth.clear();//每条直线的宽度
}

void WacomCanvas::newImage(int width,int height)//新建图像
{
    this->imageWidth=width;
    this->imageHeight=height;
    pixmap=QPixmap(imageWidth,imageHeight);
    pixmap.fill(Qt::white);
    clearOldImage();
    resize(imageWidth,imageHeight);
    update();
}

void WacomCanvas::clearOldImage()//清空原来图像
{
    for(auto shape1 : myShapes)
    {
        if(shape1!=NULL)
            delete shape1;
    }
    myShapes.clear();
    for(auto shape2 : reShapes)
    {
        if(shape2!=NULL)
            delete shape2;
    }
    reShapes.clear();

    preWidth=0.0;//前一条线的宽度
    curWidth=0.0;//当前宽度
    listPoint.clear();//当前画的直线
    listWidth.clear();//每条直线的宽度
    deviceDown = false;
}

bool WacomCanvas::saveImage(const QString &file)
{
    return pixmap.save(file);
}

bool WacomCanvas::loadImage(const QString &file)
{
    bool success = pixmap.load(file);
    imageWidth=pixmap.width();
    imageHeight=pixmap.height();
    clearOldImage();//清空原来图像
    resize(imageWidth,imageHeight);
    if (success)
    {
        update();
        return true;
    }
    return false;
}

void WacomCanvas::tabletEvent(QTabletEvent *event)
{
    QPoint temp;
    switch (event->type())
    {
        case QEvent::TabletPress:
            //点击的地方在绘图区域
            if (!pixmap.rect().contains(event->pos()))
            {
                QMessageBox::information(this,tr("绘图错误"),tr("您点击的地方不在绘图区域！"));
                return;
            }
            if (!deviceDown)
            {
                deviceDown = true;
                temp.setX(event->pos().x()-10);
                temp.setY(event->pos().y()-10);
                polyLine[0] = polyLine[1] = temp;
                addMyShape(new Pencil(polyLine[0],myColor,0.0));
                preWidth=0.0;//前一条线的宽度
                curWidth=0.0;//当前宽度
                listPoint.clear();
                listWidth.clear();
                qDebug()<<"press:"<<polyLine[0];
            }
            break;
        case QEvent::TabletRelease:
            if (deviceDown)
            {
                deviceDown = false;
                //qDebug()<<"end:"<<event->pos()<<" p:"<<polyLine[0];
            }
            break;
        case QEvent::TabletMove:
            //点击的地方在绘图区域
            if (!pixmap.rect().contains(event->pos()))
            {
                QMessageBox::information(this,tr("绘图错误"),tr("您点击的地方不在绘图区域！"));
                if (deviceDown)
                    deviceDown = false;
                return;
            }
            temp.setX(event->pos().x()-10);
            temp.setY(event->pos().y()-10);
            polyLine[1] = polyLine[0];
            polyLine[0] =temp;
            if((polyLine[0].x()==polyLine[1].x())&&(polyLine[0].y()==polyLine[1].y()))
            {
                return;
            }
            else
            {
                qDebug()<<"move:"<<polyLine[0];
                if (deviceDown)
                {
                    updateBrush(event);
                    preWidth=curWidth;
                    curWidth=myPen.widthF();
                    qDebug()<<"pW:"<<preWidth<<" cW:"<<curWidth;
                    addPointToShape();//对一段线加点
                    QPainter painter(&pixmap);
                    //打开抗锯齿性能，使图像看的更加流畅
                    painter.setRenderHint(QPainter::Antialiasing);
                    paintPixmap(painter, event);
                }
            }
            break;
        default:
            break;
    }
    update();
}

void WacomCanvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
    //qDebug()<<"fgfhgh";
}

void WacomCanvas::resizeEvent(QResizeEvent *)
{
    //initPixmap();
    polyLine[0] = polyLine[1]= QPoint();
}

//撤销
void WacomCanvas::undoShape()
{
    popMyShape();
    paintShape();
}

//重做
void WacomCanvas::redoShape()
{
    if (reShapes.empty())
    {
        QMessageBox::warning(this,tr("栈空"),tr("reShapes栈为空，不能出栈！"));
        return;
    }
    myShapes.push_back(reShapes.back());
    reShapes.pop_back();
    paintShape();
}

//清空画布
void WacomCanvas::clearAllShape()
{
    for(auto shape1 : myShapes)
    {
        if(shape1!=NULL)
            delete shape1;
    }
    myShapes.clear();
    for(auto shape2 : reShapes)
    {
        if(shape2!=NULL)
            delete shape2;
    }
    reShapes.clear();
    paintShape();
    preWidth=0.0;//前一条线的宽度
    curWidth=0.0;//当前宽度
    listPoint.clear();//当前画的直线
    listWidth.clear();//每条直线的宽度
}

//输出图像的Gcode的数据
void WacomCanvas::outPutGcode(GcodeSetData &data,const QString &file)
{
    int num=0;
    if(!file.trimmed().isEmpty())
    {
        Gcode code(data.getPenUpAngle(),data.getPenDownAngle(),data.getDelayAfterPenDown(),data.getDelayAfterPenUp(),data.getXYFeedrate(),
                   data.getZFeedrate(),data.getZHeight(),data.getZAfterHeight(),data.getXStartValue(),data.getYStartValue());
        if(myShapes.empty())
        {
            QMessageBox::information(this,tr("保存gcode数据"),tr("没有绘图数据可以保存"));
            return;
        }
        else
        {
            code.addGcodeHead(file);
            for (MyShape *shape : myShapes)
            {
                num++;
                shape->convertPoint();
                shape->getGcode(code,num);
            }
            code.addPostScript();
            if(!code.printFile(file))
            {
                QMessageBox::information(this,tr("保存gcode数据"),tr("保存gcode数据失败!"));
                return;
            }
        }
    }
}


void WacomCanvas::paintPixmap(QPainter &painter, QTabletEvent *event)
{
    //QPoint brushAdjust(10, 10);

    switch (myTabletDevice)
    {
        case QTabletEvent::Stylus:
            painter.setBrush(myBrush);
            for(int loop=0;loop<listPoint.count()-1;loop++)
            {
                myPen.setWidthF(listWidth[loop]);
                painter.setPen(myPen);
                painter.drawLine(listPoint[loop],listPoint[loop+1]);
            }
            break;
        case QTabletEvent::Airbrush:
//            myBrush.setColor(myColor);
//            myBrush.setStyle(brushPattern(event->pressure()));
//            painter.setPen(Qt::NoPen);
//            painter.setBrush(myBrush);

//            for (int i = 0; i < 3; ++i) {
//                painter.drawEllipse(QRect(polyLine[i] - brushAdjust,polyLine[i] + brushAdjust));
//            }
//            break;
        case QTabletEvent::Puck:
        case QTabletEvent::FourDMouse:
        case QTabletEvent::RotationStylus:
            {
                QMessageBox::information(this,tr("device"),tr("This input device is not supported!"));
                const QString error(tr("This input device is not supported"));
#ifndef QT_NO_STATUSTIP
                QStatusTipEvent status(error);
                QApplication::sendEvent(this, &status);
#else
                qWarning() << error;
#endif
            }
            break;
        default:
            {
                QMessageBox::information(this,tr("device"),tr("Unknown tablet device - treating as stylus"));
                const QString error(tr("Unknown tablet device - treating as stylus"));
#ifndef QT_NO_STATUSTIP
                QStatusTipEvent status(error);
                QApplication::sendEvent(this, &status);
#else
                qWarning() << error;
#endif
            }
            // FALL-THROUGH
    }
}

void WacomCanvas::updateBrush(QTabletEvent *event)
{
    int hue, saturation, value, alpha;
    myColor.getHsv(&hue, &saturation, &value, &alpha);

    int vValue = int(((event->yTilt() + 60.0) / 120.0) * 255);
    int hValue = int(((event->xTilt() + 60.0) / 120.0) * 255);

    switch (alphaChannelType)
    {
        case AlphaPressure:
            myColor.setAlpha(int(event->pressure() * 255.0));
            break;
        case AlphaTilt:
            myColor.setAlpha(maximum(abs(vValue - 127), abs(hValue - 127)));
            break;
        default:
            myColor.setAlpha(255);
    }

    switch (colorSaturationType)
    {
        case SaturationVTilt:
            myColor.setHsv(hue, vValue, value, alpha);
            break;
        case SaturationHTilt:
            myColor.setHsv(hue, hValue, value, alpha);
            break;
        case SaturationPressure:
            myColor.setHsv(hue, int(event->pressure() * 255.0), value, alpha);
            break;
        default:
            ;
    }

    switch (lineWidthType)
    {
        case LineWidthPressure:
            myPen.setWidthF(event->pressure() * widthWeight + 1);
            break;
        case LineWidthTilt:
            myPen.setWidthF(maximum(abs(vValue - 127), abs(hValue - 127)) / 12);
            break;
        default:
            myPen.setWidthF(1);
    }

    if (event->pointerType() == QTabletEvent::Eraser)
    {
        myBrush.setColor(Qt::white);
        myPen.setColor(Qt::white);
        myPen.setWidthF(event->pressure() * widthWeight + 1);
    } else
    {
        myBrush.setColor(myColor);
        myPen.setColor(myColor);
    }
}

void WacomCanvas::addMyShape(MyShape *shape)
{
    if (!shape)
    {
        QMessageBox::warning(this,tr("参数有误"),tr("所传参数shape为空指针"));
        return;
    }
    myShapes.push_back(shape);
}

void WacomCanvas::popMyShape()
{
    if (myShapes.empty())
    {
        QMessageBox::warning(this,tr("栈空"),tr("myShapes栈为空，不能出栈！"));
        return;
    }
    reShapes.push_back(myShapes.back());
    myShapes.pop_back();
}

MyShape& WacomCanvas::lastMyShape()
{
    if (myShapes.empty())
    {
        QMessageBox::warning(this,tr("栈空"),tr("myShapes栈为空，没有数据！"));
        throw std::runtime_error("No shapes exists");
    }
    return *myShapes.back();
}

//对一段线加点
void WacomCanvas::addPointToShape()
{
    int num=0;
    float detalW=0.0;
    float tempW=preWidth;
    Pencil &pencil = dynamic_cast<Pencil &>(lastMyShape());
    listPoint.clear();
    listWidth.clear();
    if(preWidth<ZERO)
    {
        if(&pencil!=NULL)
        {
            pencil.addPoint(polyLine[0]);
            pencil.addWidth(curWidth);
            listPoint.append(polyLine[1]);
            listPoint.append(polyLine[0]);
            listWidth.append(curWidth);
        }
    }
    else
    {
        if(&pencil!=NULL)
        {
            detalW=fabs(curWidth-preWidth)+0.5;
            if(detalW>1.0)
            {
                num=detalW+1;
                listPoint=pointBezier1(polyLine[1],polyLine[0],num);
                listPoint=distinctData(listPoint);//去除重复点
                num=listPoint.count();
                detalW=curWidth-preWidth;
                qDebug()<<"p1:"<<polyLine[1]<<" p2:"<<polyLine[0];
                qDebug()<<"listP:"<<listPoint;
                for(int loop=0;loop<num-1;loop++)
                {
                    tempW=tempW+detalW/(num-1);
                    pencil.addPoint(listPoint[loop+1]);
                    pencil.addWidth(tempW);
                    listWidth.append(tempW);
                }
                qDebug()<<"listW:"<<listWidth;
            }
            else
            {
                pencil.addPoint(polyLine[0]);
                pencil.addWidth(curWidth);
                listPoint.append(polyLine[1]);
                listPoint.append(polyLine[0]);
                listWidth.append(curWidth);
            }
        }
    }
}

//生成一次贝塞尔曲线点
QList<QPoint> WacomCanvas::pointBezier1(QPoint startPoint, QPoint endPoint, int numPoint)
{
    // t的增量， 可以通过setp大小确定需要保存的曲线上点的个数
    double step = 1.0/numPoint;
    QList<QPoint> list;
    QPoint point;
    double t=0.0;
    for (t = 0.0; t < 1.0; t += step)
    {
         point=Bezier1(startPoint,endPoint,t);
         list.append(point);
    }
    point=Bezier1(startPoint,endPoint,1.0);
    list.append(point);
    //qDebug()<<"一次贝塞尔曲线:"<<list;
    return list;
}

//一次贝塞尔曲线算法
QPoint WacomCanvas::Bezier1(QPoint startPoint,QPoint endPoint,double t)
{
    QPoint point;
    float x=(1.0-t)*1.0*startPoint.x()+t*1.0*endPoint.x();
    float y=(1.0-t)*1.0*startPoint.y()+t*1.0*endPoint.y();
    point.setX(x);
    point.setY(y);
    return point;
}

//绘制所有形状
void WacomCanvas::paintShape()
{
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    //打开抗锯齿性能，使图像看的更加流畅
    painter.setRenderHint(QPainter::Antialiasing);
    for (MyShape *shape : myShapes)
        shape->draw(painter);
    update();
}

//清除list中连续重复的数据
QList<QPoint> WacomCanvas::distinctData(QList<QPoint> dataList)
{
    int loop=0;
    int loop1=0;
    QList<QPoint> tempDataList;
    //qDebug()<<"data:"<<dataList;
    while(loop<dataList.count())
    {
        tempDataList.append(dataList[loop]);
        for(loop1=loop+1;loop1<dataList.count();loop1++)
        {
            if(dataList[loop]!=dataList[loop1])
            {
                break;
            }
        }
        if(loop==loop1-1)
        {
            loop++;
        }
        else
        {
            loop=loop1;
        }
    }
    //qDebug()<<"temp:"<<tempDataList;
    return tempDataList;
}

WacomCanvas::~WacomCanvas()
{
    for(auto shape1 : myShapes)
    {
        if(shape1!=NULL)
            delete shape1;
    }
    for(auto shape2 : reShapes)
    {
        if(shape2!=NULL)
            delete shape2;
    }
}

Qt::BrushStyle WacomCanvas::brushPattern(qreal value)
{
    int pattern = int((value) * 100.0) % 7;

    switch (pattern)
    {
        case 0:
            return Qt::SolidPattern;
        case 1:
            return Qt::Dense1Pattern;
        case 2:
            return Qt::Dense2Pattern;
        case 3:
            return Qt::Dense3Pattern;
        case 4:
            return Qt::Dense4Pattern;
        case 5:
            return Qt::Dense5Pattern;
        case 6:
            return Qt::Dense6Pattern;
        default:
            return Qt::Dense7Pattern;
    }
}


