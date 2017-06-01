#ifndef WACOMAPPLICATION_H
#define WACOMAPPLICATION_H

#include<QApplication>
#include"wacomcanvas.h"

class WacomApplication : public QApplication
{
    Q_OBJECT

public:
    WacomApplication(int &argv, char **args);

    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void setCanvas(WacomCanvas *canvas);

private:
    WacomCanvas *myCanvas;
};

#endif // WACOMAPPLICATION_H
