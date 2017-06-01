#include "myshape.h"
Pencil::Pencil(const QColor &color, float width) :myColor(color)
{
    widthVector.push_back(width);
    listPoint.clear();
}

Pencil::Pencil(const QPoint &point, const QColor &color, float width) :myColor(color)
{
    pointVector.push_back(point);
    widthVector.push_back(width);
    listPoint.clear();
}

Pencil::~Pencil()
{

}

void Pencil::addPoint(const QPoint &p)
{
    pointVector.push_back(p);
}

void Pencil::addWidth(float temWidth)
{
    widthVector.push_back(temWidth);
}

void Pencil::draw(QPainter &painter) const
{
    if (pointVector.size() < 2||widthVector.size()<2)
        return;

    QPen pen(myColor);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    QPen temp = painter.pen();
    painter.setPen(pen);
    for (size_t i = 1; i < pointVector.size(); i++)
    {
        pen.setWidthF(widthVector[i]);
        painter.setPen(pen);
        painter.drawLine(pointVector[i - 1], pointVector[i]);
    }

    painter.setPen(temp);
}

//对坐标点进行转换
void Pencil::convertPoint()
{
    float x=0.0;
    float y=0.0;
    QPointF point;
    listPoint.clear();
    if (pointVector.size() < 2||widthVector.size()<2)
        return;
    for(size_t loop=0;loop<pointVector.size();loop++)
    {
        x=((pointVector[loop]).x()-670)*0.28222;
        y=-((pointVector[loop]).y()-180)*0.28222;
        point.setX(x);
        point.setY(y);
        listPoint.append(point);
    }
}

//得到每条线的gcode数据
void Pencil::getGcode(Gcode &code,int num)
{
    QPointF start;
    QString str;
    str.sprintf("(Polyline consisting of %d segments.)",num);
    if(listPoint.count()<1)
    {
        return;
    }
    else
    {
        start=listPoint[0];
        code.addNote(str);
        code.goToPoint(start.x(),start.y());
        code.addStart();
    }
    for(int loop=1;loop<listPoint.count();loop++)
    {
        code.addWidth(widthVector[loop]);
        code.drawToPoint(listPoint[loop].x(),listPoint[loop].y());
    }
    code.addStop();
    code.addNote("");
}


