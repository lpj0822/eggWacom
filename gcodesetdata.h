#ifndef GCODESETDATA_H
#define GCODESETDATA_H

#include "datadef.h"

class GcodeSetData
{
public:
    GcodeSetData();
    GcodeSetData(float upAngle,float downAngle,float delayDown,float delayUp,float xyfeedrate,float zfeedrate=Z_FEEDRATE,float zheight=Z_HEIGHT,float zafterheight=Z_AFTER_HEIGHT,float x=X_START_VALUE,float y=Y_START_VALUE);

    void setPenUpAngle(float angle);
    void setPenDownAngle(float angle);
    void setDelayAfterPenDown(float delay);
    void setDelayAfterPenUp(float delay);
    void setXYFeedrate(float feedrate);
    void setZFeedrate(float feedrate);
    void setZHeight(float height);
    void setZAfterHeight(float height);
    void setXStartValue(float value);
    void setYStartValue(float value);

    float getPenUpAngle();
    float getPenDownAngle();
    float getDelayAfterPenDown();
    float getDelayAfterPenUp();
    float getXYFeedrate();
    float getZFeedrate();
    float getZHeight();
    float getZAfterHeight();
    float getXStartValue();
    float getYStartValue();

private:
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
};

#endif // GCODESETDATA_H
