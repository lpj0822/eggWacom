#ifndef MYSHAPE_H
#define MYSHAPE_H

#include <QPoint>
#include <QColor>
#include <QPainter>
#include<QPointF>
#include"gcode.h"

class MyShape
{
public:
  /**
   * @brief draw
   * @detailed draws appropriate Shape
   * @param QPainter QPainter which will be used for drawing
   */
    virtual void draw(QPainter &) const = 0;
    virtual void convertPoint() =0;
    virtual void getGcode(Gcode &code,int num) =0;
   /**
    * @brief ~Shape
    * @detailed Destructor (virtual, empty)
    */
    virtual ~MyShape() {}
};

class Pencil : public MyShape
{
private:
    std::vector<QPoint> pointVector;
    std::vector<float> widthVector;
    QList<QPointF> listPoint;//准换后的实际坐标
    QColor myColor;

public:
    Pencil(const QPoint &, const QColor &, float width);
    Pencil(const QColor &, float width);
    ~Pencil();
   /**
    * @brief addPoit
    * @detailed adds new point to figure, that will be connected
    * by line with previous point
    * @param point new point
    */
    void addPoint(const QPoint &);
    void addWidth(float temWidth);
    void draw(QPainter &) const;
    void convertPoint();//对坐标点进行转换
    void getGcode(Gcode &code,int num);//得到每条线的gcode数据

};

#endif // MYSHAPE_H
