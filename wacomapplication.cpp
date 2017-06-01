#include "wacomapplication.h"
#include <QtWidgets>

WacomApplication::WacomApplication(int &argv, char **args): QApplication(argv, args)
{
}

bool WacomApplication::event(QEvent *event)
{
    if (event->type() == QEvent::TabletEnterProximity ||event->type() == QEvent::TabletLeaveProximity)
    {
          myCanvas->setTabletDevice(static_cast<QTabletEvent *>(event)->device());
          return true;
    }
    return QApplication::event(event);
}

void WacomApplication::setCanvas(WacomCanvas *canvas)
{
    myCanvas=canvas;
}

