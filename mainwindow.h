#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QStatusBar>
#include "datadef.h"
#include "wacomcanvas.h"
#include "gcodeset.h"
#include "gcodesetdata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(WacomCanvas *canvas,QWidget *parent = 0);
    ~MainWindow();

private slots:
    void penWidthAct();
    void brushColorAct();
    void alphaActionTriggered(QAction *action);
    void lineWidthActionTriggered(QAction *action);
    void saturationActionTriggered(QAction *action);
    void saveAct();
    void loadAct();
    void newAct();
    void saveGcodeAct();
    void printGcodeAct();
    void undoAct();
    void redoAct();
    void clearAct();
    void aboutAct();

private:
    Ui::MainWindow *ui;

    void initUI();//初始化UI
    void initData();//初始化数据

    void createActions();
    void createMenus();
    void createToolBars();

    QScrollArea *scrollArea;//滚动区域

    WacomCanvas *myCanvas;

    QAction *penWidthAction;
    QAction *brushColorAction;
    QActionGroup *brushActionGroup;

    QActionGroup *alphaChannelGroup;
    QAction *alphaChannelPressureAction;
    QAction *alphaChannelTiltAction;
    QAction *noAlphaChannelAction;

    QActionGroup *colorSaturationGroup;
    QAction *colorSaturationVTiltAction;
    QAction *colorSaturationHTiltAction;
    QAction *colorSaturationPressureAction;
    QAction *noColorSaturationAction;

    QActionGroup *lineWidthGroup;
    QAction *lineWidthPressureAction;
    QAction *lineWidthTiltAction;
    QAction *lineWidthFixedAction;

    QAction *exitAction;
    QAction *saveAction;
    QAction *loadAction;
    QAction *newAction;//新建

    QAction *saveGcodeAction;//保存为Gcode代码
    QAction *printGcodeAction;//打印Gcode文件

    QAction *undoAction;//撤销
    QAction *redoAction;//重做
    QAction *clearAction;//清空画布

    QAction *aboutAction;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *penMenu;
    QMenu *tabletMenu;
    QMenu *helpMenu;
    QMenu *colorSaturationMenu;
    QMenu *lineWidthMenu;
    QMenu *alphaChannelMenu;

    QToolBar *fileTool;
    QToolBar *editTool;
    QToolBar *colorTool;

    GcodeSetData setData;//gcode代码参数
};

#endif // MAINWINDOW_H
