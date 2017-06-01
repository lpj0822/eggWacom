#include <QtWidgets>
#include "mainwindow.h"
#include "widthsetdialog.h"
#include "spherebot.h"
#include "newdialog.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(WacomCanvas *canvas,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //this->resize(WINDOWWIDTH, WINDOWHEIGHT);
    initData();//初始化化数据
    myCanvas = canvas;
    createActions();
    createMenus();
    createToolBars();

    myCanvas->setColor(Qt::red);
    myCanvas->setLineWidthType(WacomCanvas::LineWidthPressure);
    myCanvas->setAlphaChannelType(WacomCanvas::NoAlpha);
    myCanvas->setColorSaturationType(WacomCanvas::NoSaturation);

    initUI();//初始化UI
    setWindowTitle(tr("Egg wacom"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


//画笔宽度权值设置
void MainWindow::penWidthAct()
{
    int weight=myCanvas->getWidthWeight();
    WidthSetDialog widthSet(weight,this);
    if(widthSet.exec() == QDialog::Accepted)
    {
        weight=widthSet.getValue();
        myCanvas->setWidthWeight(weight);
    }
}

void MainWindow::brushColorAct()
{
    QColor color = QColorDialog::getColor(myCanvas->color());

    if (color.isValid())
    {
        myCanvas->setColor(color);
    }
}

void MainWindow::alphaActionTriggered(QAction *action)
{
    if (action == alphaChannelPressureAction)
    {
        myCanvas->setAlphaChannelType(WacomCanvas::AlphaPressure);
    } else if (action == alphaChannelTiltAction)
    {
        myCanvas->setAlphaChannelType(WacomCanvas::AlphaTilt);
    } else
    {
        myCanvas->setAlphaChannelType(WacomCanvas::NoAlpha);
    }
}

void MainWindow::lineWidthActionTriggered(QAction *action)
{
    if (action == lineWidthPressureAction) {
        myCanvas->setLineWidthType(WacomCanvas::LineWidthPressure);
    } else if (action == lineWidthTiltAction) {
        myCanvas->setLineWidthType(WacomCanvas::LineWidthTilt);
    } else {
        myCanvas->setLineWidthType(WacomCanvas::NoLineWidth);
    }
}

void MainWindow::saturationActionTriggered(QAction *action)
{
    if (action == colorSaturationVTiltAction)
    {
        myCanvas->setColorSaturationType(WacomCanvas::SaturationVTilt);
    } else if (action == colorSaturationHTiltAction)
    {
        myCanvas->setColorSaturationType(WacomCanvas::SaturationHTilt);
    } else if (action == colorSaturationPressureAction)
    {
        myCanvas->setColorSaturationType(WacomCanvas::SaturationPressure);
    } else
    {
        myCanvas->setColorSaturationType(WacomCanvas::NoSaturation);
    }
}

void MainWindow::saveAct()
{
    QString path = QDir::currentPath() + "/untitled.png";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Picture"),path,tr("Image Files (*.png *.jpg *.bmp)"));
    if(!fileName.trimmed().isEmpty())
    {
        if (!myCanvas->saveImage(fileName))
        {
            QMessageBox::information(this, "Error Saving Picture","Could not save the image");
        }
    }
}

void MainWindow::loadAct()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Picture"),QDir::currentPath(),tr("Image Files (*.png *.jpg *.bmp)"));
    if(!fileName.trimmed().isEmpty())
    {
        if (!myCanvas->loadImage(fileName))
        {
            QMessageBox::information(this, "Error Opening Picture","Could not open picture");
        }
    }
}

//新建画布
void MainWindow::newAct()
{
    int canvas_w=myCanvas->getCanvasWidth();
    int canvas_h=myCanvas->getCanvasHeight();
    NewDialog newDialog(canvas_w,canvas_h,this);
    if(newDialog.exec()==QDialog::Accepted)
    {
        canvas_w=newDialog.getWidth();
        canvas_h=newDialog.getHeight();
        myCanvas->newImage(canvas_w,canvas_h);
    }
}

//保存Gcode代码
void MainWindow::saveGcodeAct()
{
    QString path = QDir::currentPath() + "/untitled.gcode";
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save Gcode"),path,"G-code files(*.gcode)");
    GcodeSet set(this,setData.getPenUpAngle(),setData.getPenDownAngle(),setData.getDelayAfterPenDown(),setData.getDelayAfterPenUp(),
                 setData.getXYFeedrate(),setData.getZFeedrate(),setData.getZHeight(),setData.getZAfterHeight(),setData.getXStartValue(),
                 setData.getYStartValue());
    if(!fileName.trimmed().isEmpty())
    {
        if(set.exec() == QDialog::Accepted)
        {
            setData=set.getData();
            myCanvas->outPutGcode(setData,fileName);
        }
    }
}

//打印Gcode文件
void MainWindow::printGcodeAct()
{
    SphereBot *printUI=new SphereBot(this);
    printUI->setWindowModality(Qt::NonModal);
    printUI->show();
}

//撤销
void MainWindow::undoAct()
{
   myCanvas->undoShape();
}

//重做
void MainWindow::redoAct()
{
    myCanvas->redoShape();
}

//清空画布
void MainWindow::clearAct()
{
    myCanvas->clearAllShape();
}

void MainWindow::aboutAct()
{
    QMessageBox::about(this, tr("About Egg wacom"),tr("This application shows use of a Wacom tablet in Qt"));
}

void MainWindow::createActions()
{
    penWidthAction=new QAction(tr("&Pen Width"),this);
    penWidthAction->setShortcut(tr("Ctrl+W"));
    connect(penWidthAction,SIGNAL(triggered()),this,SLOT(penWidthAct()));

    brushColorAction = new QAction(tr("&Brush Color..."), this);
    brushColorAction->setShortcut(tr("Ctrl+C"));
    connect(brushColorAction, SIGNAL(triggered()),this, SLOT(brushColorAct()));

    alphaChannelPressureAction = new QAction(tr("&Pressure"), this);
    alphaChannelPressureAction->setCheckable(true);

    alphaChannelTiltAction = new QAction(tr("&Tilt"), this);
    alphaChannelTiltAction->setCheckable(true);

    noAlphaChannelAction = new QAction(tr("No Alpha Channel"), this);
    noAlphaChannelAction->setCheckable(true);
    noAlphaChannelAction->setChecked(true);

    alphaChannelGroup = new QActionGroup(this);
    alphaChannelGroup->addAction(alphaChannelPressureAction);
    alphaChannelGroup->addAction(alphaChannelTiltAction);
    alphaChannelGroup->addAction(noAlphaChannelAction);
    connect(alphaChannelGroup, SIGNAL(triggered(QAction*)),this, SLOT(alphaActionTriggered(QAction*)));

    colorSaturationVTiltAction = new QAction(tr("&Vertical Tilt"), this);
    colorSaturationVTiltAction->setCheckable(true);

    colorSaturationHTiltAction = new QAction(tr("&Horizontal Tilt"), this);
    colorSaturationHTiltAction->setCheckable(true);

    colorSaturationPressureAction = new QAction(tr("&Pressure"), this);
    colorSaturationPressureAction->setCheckable(true);

    noColorSaturationAction = new QAction(tr("&No Color Saturation"), this);
    noColorSaturationAction->setCheckable(true);
    noColorSaturationAction->setChecked(true);

    colorSaturationGroup = new QActionGroup(this);
    colorSaturationGroup->addAction(colorSaturationVTiltAction);
    colorSaturationGroup->addAction(colorSaturationHTiltAction);
    colorSaturationGroup->addAction(colorSaturationPressureAction);
    colorSaturationGroup->addAction(noColorSaturationAction);
    connect(colorSaturationGroup, SIGNAL(triggered(QAction*)),this, SLOT(saturationActionTriggered(QAction*)));

    lineWidthPressureAction = new QAction(tr("&Pressure"), this);
    lineWidthPressureAction->setCheckable(true);
    lineWidthPressureAction->setChecked(true);

    lineWidthTiltAction = new QAction(tr("&Tilt"), this);
    lineWidthTiltAction->setCheckable(true);

    lineWidthFixedAction = new QAction(tr("&Fixed"), this);
    lineWidthFixedAction->setCheckable(true);

    lineWidthGroup = new QActionGroup(this);
    lineWidthGroup->addAction(lineWidthPressureAction);
    lineWidthGroup->addAction(lineWidthTiltAction);
    lineWidthGroup->addAction(lineWidthFixedAction);
    connect(lineWidthGroup, SIGNAL(triggered(QAction*)),this, SLOT(lineWidthActionTriggered(QAction*)));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()),this, SLOT(close()));

    loadAction = new QAction(tr("&Open..."), this);
    loadAction->setShortcuts(QKeySequence::Open);
    connect(loadAction, SIGNAL(triggered()),this, SLOT(loadAct()));

    saveAction = new QAction(tr("&Save As Picture"), this);
    saveAction->setShortcuts(QKeySequence::SaveAs);
    connect(saveAction, SIGNAL(triggered()),this, SLOT(saveAct()));

    newAction=new QAction(tr("&New canvas"),this);
    newAction->setShortcuts(QKeySequence::New);
    connect(newAction,SIGNAL(triggered()),this,SLOT(newAct()));//新建画布

    saveGcodeAction=new QAction(tr("&Save As GCode"),this);//保存为Gcode代码
    saveGcodeAction->setShortcut(tr("Ctrl+G"));
    connect(saveGcodeAction,SIGNAL(triggered()),this,SLOT(saveGcodeAct()));

    printGcodeAction=new QAction(tr("&Print Gcode"),this);//打印Gcode文件
    printGcodeAction->setShortcut(tr("Ctrl+P"));
    connect(printGcodeAction,SIGNAL(triggered()),this,SLOT(printGcodeAct()));

    undoAction=new QAction(tr("&Undo"),this);//撤销
    undoAction->setShortcut(tr("Ctrl+Z"));
    connect(undoAction,SIGNAL(triggered()),this,SLOT(undoAct()));

    redoAction=new QAction(tr("&Redo"),this);//重做
    redoAction->setShortcut(tr("Ctrl+Y"));
    connect(redoAction,SIGNAL(triggered()),this,SLOT(redoAct()));

    clearAction=new QAction(tr("&Clear All"),this);
    clearAction->setShortcut(tr("Ctrl+A"));
    connect(clearAction,SIGNAL(triggered()),this,SLOT(clearAct()));

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("Ctrl+B"));
    connect(aboutAction, SIGNAL(triggered()),this, SLOT(aboutAct()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveGcodeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(printGcodeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu=menuBar()->addMenu(tr("&Edit"));//编辑菜单
    editMenu->addAction(undoAction);//撤销
    editMenu->addAction(redoAction);//重做
    editMenu->addAction(clearAction);//清空画布

    penMenu = menuBar()->addMenu(tr("&Pen"));
    penMenu->addAction(penWidthAction);
    penMenu->addAction(brushColorAction);

    tabletMenu = menuBar()->addMenu(tr("&Tablet"));

    lineWidthMenu = tabletMenu->addMenu(tr("&Line Width"));
    lineWidthMenu->addAction(lineWidthPressureAction);
    lineWidthMenu->addAction(lineWidthTiltAction);
    lineWidthMenu->addAction(lineWidthFixedAction);

    alphaChannelMenu = tabletMenu->addMenu(tr("&Alpha Channel"));
    alphaChannelMenu->addAction(alphaChannelPressureAction);
    alphaChannelMenu->addAction(alphaChannelTiltAction);
    alphaChannelMenu->addAction(noAlphaChannelAction);

    colorSaturationMenu = tabletMenu->addMenu(tr("&Color Saturation"));
    colorSaturationMenu->addAction(colorSaturationVTiltAction);
    colorSaturationMenu->addAction(colorSaturationHTiltAction);
    colorSaturationMenu->addAction(noColorSaturationAction);

    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(aboutAction);
}

//创造工具栏
void MainWindow::createToolBars()
{
    fileTool=addToolBar(tr("File"));
    fileTool->addAction(loadAction);
    fileTool->addAction(saveGcodeAction);
    fileTool->addAction(printGcodeAction);

    editTool=addToolBar(tr("Eidt"));
    editTool->addAction(undoAction);
    editTool->addAction(redoAction);

    colorTool=addToolBar(tr("Color"));
    colorTool->addAction(brushColorAction);
}

//初始化数据
void MainWindow::initData()
{
    setData.setPenUpAngle(PEN_UP_ANGLE);
    setData.setPenDownAngle(PEN_DOWN_ANGLE);
    setData.setDelayAfterPenDown(DELAY_PEN_DOWN);
    setData.setDelayAfterPenUp(DELAY_PEN_UP);
    setData.setXYFeedrate(XY_FEEDRATE);
    setData.setZFeedrate(Z_FEEDRATE);
    setData.setZHeight(Z_HEIGHT);
    setData.setZAfterHeight(Z_AFTER_HEIGHT);
    setData.setXStartValue(X_START_VALUE);
    setData.setYStartValue(Y_START_VALUE);
}

//初始化UI
void MainWindow::initUI()
{
    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(myCanvas);
    //scrollArea->widget()->setMinimumSize(800,600);
    setCentralWidget(scrollArea);
    this->setMaximumSize(MAINWIDTH,MAINHEIGHT);
    this->setMinimumSize(MAINWIDTH,MAINHEIGHT);
}
