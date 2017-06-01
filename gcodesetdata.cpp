#include "gcodesetdata.h"

GcodeSetData::GcodeSetData()
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
}

GcodeSetData::GcodeSetData(float upAngle,float downAngle,float delayDown,float delayUp,float xyfeedrate,float zfeedrate,float zheight,float zafterheight,float x,float y)
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
}

void GcodeSetData::setPenUpAngle(float angle)
{
    this->penUpAngle=angle;
}

void GcodeSetData::setPenDownAngle(float angle)
{
    this->penDownAngle=angle;
}

void GcodeSetData::setDelayAfterPenDown(float delay)
{
    this->delayAfterPenDown=delay;
}

void GcodeSetData::setDelayAfterPenUp(float delay)
{
    this->delayAfterPenUp=delay;
}

void GcodeSetData::setXYFeedrate(float feedrate)
{
    this->xyFeedrate=feedrate;
}

void GcodeSetData::setZFeedrate(float feedrate)
{
    this->zFeedrate=feedrate;
}

void GcodeSetData::setZHeight(float height)
{
    this->zHeight=height;
}

void GcodeSetData::setZAfterHeight(float height)
{
    this->zAfterHeight=height;
}

void GcodeSetData::setXStartValue(float value)
{
    this->xStartValue=value;
}

void GcodeSetData::setYStartValue(float value)
{
    this->yStartValue=value;
}

float GcodeSetData::getPenUpAngle()
{
    return this->penUpAngle;
}

float GcodeSetData::getPenDownAngle()
{
    return this->penDownAngle;
}

float GcodeSetData::getDelayAfterPenDown()
{
    return this->delayAfterPenDown;
}

float GcodeSetData::getDelayAfterPenUp()
{
    return this->delayAfterPenUp;
}

float GcodeSetData::getXYFeedrate()
{
    return this->xyFeedrate;
}

float GcodeSetData::getZFeedrate()
{
    return this->zFeedrate;
}

float GcodeSetData::getZHeight()
{
    return this->zHeight;
}

float GcodeSetData::getZAfterHeight()
{
    return this->zAfterHeight;
}

float GcodeSetData::getXStartValue()
{
    return this->xStartValue;
}

float GcodeSetData::getYStartValue()
{
    return this->yStartValue;
}


