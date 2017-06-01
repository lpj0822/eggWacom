#include "mainwindow.h"
#include <QApplication>
#include"wacomapplication.h"
#include"wacomcanvas.h"

int main(int argc, char *argv[])
{
    WacomApplication a(argc, argv);
    a.setOrganizationName("SpherebotUI");
    WacomCanvas *canvas=new WacomCanvas;
    a.setCanvas(canvas);
    MainWindow w(canvas);
    w.show();

    return a.exec();
}
