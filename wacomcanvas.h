#ifndef WACOMCANVAS_H
#define WACOMCANVAS_H

#include <QWidget>
#include <QPixmap>
#include <QTabletEvent>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QPoint>
#include <QPaintEvent>
#include <QString>
#include <QList>
#include <QScrollArea>
#include "datadef.h"
#include "myshape.h"
#include "gcodesetdata.h"
#include "gcode.h"

class WacomCanvas : public QWidget
{
    Q_OBJECT
public:
    enum AlphaChannelType { AlphaPressure, AlphaTilt, NoAlpha };
    enum ColorSaturationType { SaturationVTilt, SaturationHTilt,SaturationPressure, NoSaturation };
    enum LineWidthType { LineWidthPressure, LineWidthTilt, NoLineWidth };

    WacomCanvas(QWidget *parent = 0);
    ~WacomCanvas();

    void undoShape();//撤销
    void redoShape();//重做
    void clearAllShape();//清除画布

    void addMyShape(MyShape *shape);//添加形状
    void popMyShape();//出栈形状
    MyShape& lastMyShape();//返回最后一个形状

    void newImage(int width,int height);//新建图像

    bool saveImage(const QString &file);
    bool loadImage(const QString &file);
    //输出图像的Gcode的数据
    void outPutGcode(GcodeSetData &data,const QString &file);

    void setAlphaChannelType(AlphaChannelType type)
    {
        alphaChannelType = type;
    }

    void setColorSaturationType(ColorSaturationType type)
    {
        colorSaturationType = type;
    }

    void setLineWidthType(LineWidthType type)
    {
        lineWidthType = type;
    }

    void setColor(const QColor &color)
    {
        myColor = color;
    }

    QColor color() const
    {
        return myColor;
    }

    void setTabletDevice(QTabletEvent::TabletDevice device)
    {
        myTabletDevice = device;
    }

    int maximum(int a, int b)
    {
        return a > b ? a : b;
    }

    //设置宽度权值
    void setWidthWeight(int weight)
    {
        this->widthWeight=weight;
    }

    //得到宽度权值权值
    int getWidthWeight()
    {
        return widthWeight;
    }

    //得到画布宽度
    int getCanvasWidth()
    {
        return imageWidth;
    }

    //得到画布高度
    int getCanvasHeight()
    {
        return imageHeight;
    }

signals:

public slots:

protected:
    void tabletEvent(QTabletEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    void initData();//数据初始化
    void initPixmap();//初始化图像
    void clearOldImage();//清空原来图像
    void paintPixmap(QPainter &painter, QTabletEvent *event);
    Qt::BrushStyle brushPattern(qreal value);
    void updateBrush(QTabletEvent *event);

    void paintShape();//绘制所有形状

    void addPointToShape();//对一段线加点
    //生成一次贝塞尔曲线点
    QList<QPoint> pointBezier1(QPoint startPoint,QPoint endPoint,int numPoint);
    //一次贝塞尔曲线算法
    QPoint Bezier1(QPoint startPoint,QPoint endPoint,double t);
    //清除list中连续重复的数据
    QList<QPoint> distinctData(QList<QPoint> dataList);

    AlphaChannelType alphaChannelType;
    ColorSaturationType colorSaturationType;
    LineWidthType lineWidthType;

    QTabletEvent::PointerType pointerType;
    QTabletEvent::TabletDevice myTabletDevice;
    QColor myColor;

    QPixmap pixmap;
    QBrush myBrush;
    QPen myPen;
    bool deviceDown;//是否按下
    QPoint polyLine[2];

    int widthWeight;//宽度权值

    float preWidth;//前一线的宽度
    float curWidth;//当前宽度

    QList<QPoint> listPoint;//画线的点
    QList<float> listWidth;//每点之间的宽度

    std::list <MyShape*> myShapes;//绘图数据
    std::list <MyShape*> reShapes;//用于恢复的绘图数据

    int imageWidth;//图片宽度
    int imageHeight;//图片宽度
};

#endif // WACOMCANVAS_H
