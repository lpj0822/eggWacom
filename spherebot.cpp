#include "spherebot.h"
#include "ui_spherebot.h"

SphereBot::SphereBot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SphereBot)
{
    ui->setupUi(this);
    resetPortList();//刷新可用串口名列表

    // for .svg display
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    isCon=false;
    sendState = Idle;//空闲状态
    isSendButton=true;
    this->bot = new SpherebotPort();//串口操作

    penDownAngle = DEFAULTPENDOWN;
    penUpAngle = DEFAULTPENUP;

    layerIndex = 0;//当前图层索引
    initUI();//初始化对话框
    this->setWindowTitle("SpherebotUI control");

    connect( this->bot, SIGNAL(dataSent(QString)),this, SLOT(sendDataUI(QString)));
    connect(&transceiver,SIGNAL(progressChanged(int)),this,SLOT(refreshSendProgress(int)));
    connect(&transceiver,SIGNAL(fileTransmitted()),this,SLOT(finishedTransmission()));
    connect(this->bot,SIGNAL(dataSent(QString)),this,SLOT(interpretSentString(QString)));
    connect(&transceiver,SIGNAL(layerTransmitted()),this,SLOT(finishedLayer()));

    //加载Settings表中的信息,是否是第一次运行
    if(LoadSettings())
    {
        //设置超时时间
        FitInTimer.setInterval(10);
        //仅仅启动定时器一次
        FitInTimer.setSingleShot(true);
        connect(&FitInTimer,SIGNAL(timeout()),this,SLOT(fitgraphicsView()));
        FitInTimer.start();
    }
    qDebug()<<"sphereBot initialised: ";
}

SphereBot::~SphereBot()
{
    qDebug()<<"delete sphereBot";
    SaveSettings();
    delete ui;
}

//初始化对话框
void SphereBot::initUI()
{
    nextLayerMsgBox = new QMessageBox(QMessageBox::Information,
                                      "Next Layer",
                                      "The Layer has been finished!\nplease insert the tool for the layer: " + QString::number(layerIndex),
                                      QMessageBox::Yes|QMessageBox::No);
    nextLayerMsgBox->setButtonText(QMessageBox::Yes,"OK");
    nextLayerMsgBox->setButtonText(QMessageBox::No,"Abort");

    restartLayerMsgBox = new QMessageBox(QMessageBox::Information,
                                         "Restart?",
                                         "Do you want to restart the print?",
                                         QMessageBox::Yes|QMessageBox::No);
    //restartLayerMsgBox->setButtonText(QMessageBox::Yes,"OK");
    //restartLayerMsgBox->setButtonText(QMessageBox::No,"No");
}

//function to trigger the fitIn function for the graphics view. Actually this shouldn´t be necessary!
//显示svg
void SphereBot::fitgraphicsView()
{
    QGraphicsItem *item = ui->graphicsView->items().first();
    ui->graphicsView->fitInView(item);

    ui->graphicsView->ensureVisible(item);
    qDebug()<<"fit in";
}

//加载Settings表中的信息
bool SphereBot::LoadSettings()
{
    bool returnvalue = false;
    QSettings settings("SpherebotUI","SpherebotUISettings");
    settings.beginGroup("settings");
    curFile = settings.value("fileName", "").toString();
    curDir = settings.value("currentDirectory", "").toString();
//    if(!curFile.isEmpty())
//    {
//        if(QFile::exists(curFile))
//        {
//            qDebug()<<"load last file.";
//            loadFile(curFile);
//            returnvalue = true;
//        }
//    }
    qDebug()<<"load: "<<curFile;
    QString SavedPortName = settings.value("PortName", "").toString();
    QSerialPortInfo info;
    portList = info.availablePorts();
    for(int i = 0; i < portList.size();i++)
    {
        if(QString::compare(portList.at(i).portName(),SavedPortName) == 0)
        {
            ui->portBox->setCurrentIndex(i);
            bot->port->setPortName(SavedPortName);
            on_connectButton_clicked();
            ui->sendButton->setEnabled(true);
        }
    }
    settings.endGroup();
    qDebug()<<"settings loaded: ";
    return returnvalue;
}

//保存信息到Settings表中
void SphereBot::SaveSettings()
{
    QSettings settings("SpherebotUI","SpherebotUISettings");
    settings.beginGroup("settings");
    settings.setValue("fileName", curFile);
    settings.setValue("currentDirectory",curDir);
    settings.setValue("PortName", ui->portBox->currentText());
    settings.endGroup();
}

//得到文件中的参数
void SphereBot::extractOptions(QString file)
{
    penUpAngle = getOption(file,"--pen-up-angle=");//得到文件头里的抬笔角度
    penDownAngle = getOption(file,"--pen-down-angle=");//得到文件头里的落笔角度
    qDebug()<<"penup: "<<penUpAngle;
    qDebug()<<"pendown: "<<penDownAngle;
}

//根据文件名加载文件
void SphereBot::loadFile(const QString &fileName)
{
    qDebug()<<"loading file: "<<fileName;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }
    //返回包含文件名的绝对路径
    //curDir = QFileInfo(fileName).absoluteFilePath();
    //返回文件的绝对路径，不包含文件名
    curDir=QFileInfo(fileName).absolutePath();
    curFile=fileName;
    statusBar()->showMessage(tr("File loaded"), 2000);

    QString code = file.readAll();
    extractOptions(code);//得到文件中抬笔和落笔的参数
    interpretGcode(code);//for future gcode interpretation
    refreshLayerNames(code);//得到每个图层的名字
    ui->fileTextEdit->setText(code);
    qDebug()<<removeComments(code);

    //显示svg
    scene->clear();//场景清空
    QString picPath = QFileInfo(fileName).absoluteFilePath();
    picPath.chop(5);        //cut .gcode
    picPath.append("svg");
    QGraphicsSvgItem *item = new QGraphicsSvgItem(picPath);
    scene->addItem(item);
    ui->graphicsView->setEnabled(true);
    ui->graphicsView->fitInView(item);

    //显示文件名
    if(!ui->fileTextEdit->toPlainText().isEmpty())
    {
        setWindowTitle("Spherebot Control      File: " + fileName);
        ui->fileName->setText(QFileInfo(fileName).fileName());
    }
    else
        ui->sendButton->setEnabled(false);
}

//保存文件
bool SphereBot::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);

    out<<ui->fileTextEdit->toPlainText();
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

//for future gcode interpretation
void SphereBot::interpretGcode(QString code)
{
    code = removeComments(code);//删除空白与g-code的注释部分
    QStringList lines = code.split("\n");
    QStringList line;
    int state = 0; // 0 = pen up , 1 = pen down
    for(int i = 0;i<lines.length();i++)
    {
        //qDebug()<<lines[i];
        line = lines[i].split(" ");
        if(lines[i].contains("M300 S" + QString::number(penUpAngle) +".00"))
            state = 0;
        else if(lines[i].contains("M300 S" + QString::number(penDownAngle) +".00"))
            state = 1;

        if(state == 1)//drawing
        {

        }
    }
}

//通过M01得到每个图层的名字
void SphereBot::refreshLayerNames(QString file)
{
    QList<QString> lines = file.split("\n");
    int layerchange = 0;
    for(int i=0;i<lines.size();i++)
    {
        qDebug()<<lines[i];
        if(lines[i].contains("M01"))
        {
            if(layerchange != 0)
            {
                qDebug()<<"This is the Split Line: " << lines[i].split("'");
                layerNames.append(lines[i].split("'")[1]);
            }
            layerchange++;
        }
    }
    layerIndex=0;
    qDebug()<<"This are the layernames: "<< layerNames;
}

//接收串口传来的数据,显示在接收窗口中
void SphereBot::receiveData()
{
    if(bot->port->canReadLine())
    {
        QString line=bot->port->readAll();
        //QString line = bot->port->readLine(1024);
        line.chop(1);
        if(line!="")
        {
            ui->rxList->insertItem(0,line);
            delete ui->rxList->item(MAXLISTITEMS);
            if(isSendButton!=true)
                this->transceiver.sendNext();
        }
        bot->port->flush();
    }
}

//刷新发送数据的进度
void SphereBot::refreshSendProgress(int value)
{
    ui->fileSendProgressBar->setValue(value);
}

//图层改变时的操作
void SphereBot::finishedLayer()
{
//    if(layerNames.size() > 1)
//    {
//        layerIndex++;
//    }
    qDebug()<<"layerIndex: "<<layerIndex;
    qDebug()<<"layerNames: "<<layerNames;
    qDebug()<<"layerNames.size(): "<<layerNames.size();
    if(layerIndex < layerNames.size())      //next layer
    {
        //提示图层改变
        nextLayerMsgBox->setText("Please change the tool for layer: " + layerNames[layerIndex]);
        SetBotToHomePosition();
        switch(nextLayerMsgBox->exec())
        {
            case(QMessageBox::No):
            setState(Idle);    //abort
            break;
            case(QMessageBox::Yes):
            break;
            default:
            setState(Idle);    //abort
            break;
        }
        layerIndex++;
    }
}

//传输数据完成时的操作
void SphereBot::finishedTransmission()
{
    disconnectTranceiver();
    this->transceiver.resetState();
    ui->sendFileButton->setText("Send File");
    ui->loadFileButton->setText("Load File");
    ui->controllBox->setEnabled(true);//电机控制模块可用
    ui->resetButton->setEnabled(false);//串口刷新按钮不可用
    ui->fileSendProgressBar->setValue(0);
    ui->fileSendProgressBar->setEnabled(false);
    ui->loadFileButton->setEnabled(true);
    statusBar()->showMessage(tr("File successfully sent"));
    sendState = Idle;
    layerIndex = 0;
    isSendButton=true;
    //SetBotToHomePosition();//发送Y方向的直线运动到0并抬笔命令
    //是否重新发送文件
    if (QMessageBox::Yes == restartLayerMsgBox->exec())
    {
        on_sendFileButton_clicked();//点击发送文件按钮如果是空闲状态则发送数据，如果是发送状态则停止，如果是停止则继续发送
    }
}

//根据串口发送的数据更新控制电机的实时更新点击的界面
void SphereBot::interpretSentString(QString string)
{
    if(this->sendState == 1)        //if currently sending
    {
        QStringList list = string.split(" ");
        //qDebug()<<"string is : "<<string;
        for(int i = 0;i<list.size();i++)
        {
            if(!list[i].isEmpty())
            {
                if (list[i].startsWith('X'))
                {
                    //qDebug()<<"setting eggslidervalue: ";
                    //x轴的位置
                    ui->eggSlider->setValue(list[i].remove(0,1).toDouble());
                }
                else if (list[i].startsWith('Y'))
                {
                    //qDebug()<<"setting penslidervalue";
                    //y轴的位置
                    ui->penSlider->setValue(list[i].remove(0,1).toDouble());
                }
                else if (list[i].startsWith('M'))
                {
                    if(list[i].remove(0,1) == "300")
                    {
                        //qDebug()<<"setting servoSlider";
                        //舵机角度
                        ui->servoSlider->setValue(list[i+1].remove(0,1).toDouble());
                    }
                    else if(list[i].remove(0,1) == "400")
                    {
                        // qDebug()<<"setting diameterSlider";
                        //球状物直径设置
                        ui->diameterSlider->setValue(list[i+1].remove(0,1).toDouble());
                    }
                }
                else if (list[i].startsWith('F'))
                {
                    //qDebug()<<"setting servoFeedrateSlider";
                    //舵机的速度设置
                    ui->servoFeedrateSlider->setValue(list[i].remove(0,1).toDouble());
                }
            }
        }
    }
}

//串口如果打开则断开,如果断开者打开
void SphereBot::on_connectButton_clicked()
{
    if(bot->isConnected())
    {
        //disconnect
        bot->disconnectWithBot();
        ui->connectButton->setChecked(false);
        ui->connectButton->setText("Connect");
        ui->controllBox->setEnabled(false);
        ui->resetButton->setEnabled(true);
        ui->portBox->setEnabled(true);
        ui->sendFileButton->setEnabled(false);
        ui->eggSlider->setValue(0);
        ui->diameterSlider->setValue(DEFAULTDIAMETER);
        ui->penSlider->setValue(0);
        ui->servoSlider->setValue(DEFAULTPENUP);
        ui->FeedratespinBox->setValue(DEFAULTFEEDRATE);
        ui->fileSendProgressBar->setValue(0);
        sendState = Idle;
        ui->sendFileButton->setText("Send File");
        ui->sendFileButton->setEnabled(false);
    }
    else if(bot->connectWithBot(ui->portBox->currentText()))
    {
        //successfully connected
        //connect( this->bot->port, SIGNAL(readyRead()), this, SLOT(receiveData()));
        if(isCon!=true)
            connectTranceiver();
        ui->connectButton->setChecked(true);
        ui->controllBox->setEnabled(true);
        ui->portBox->setEnabled(false);
        ui->resetButton->setEnabled(false);
        ui->connectButton->setText("Disconnect");
        if(!ui->fileTextEdit->toPlainText().isEmpty())
            ui->sendFileButton->setEnabled(true);
    }
    else
    {

    }
}

//刷新串口信息
void SphereBot::resetPortList()
{
    QSerialPortInfo info;
    portList = info.availablePorts();
    ui->portBox->clear();
    for(int i=0;i<portList.size();i++)
    {
        ui->portBox->addItem(portList[i].portName());
    }
}

//刷新串口信息
void SphereBot::on_resetButton_clicked()
{
    resetPortList();
}

//串口发送出数据后，更新显示发送数据的界面
void SphereBot::sendDataUI(QString data)
{
    ui->txList->insertItem(0,data);
    delete ui->txList->item(MAXLISTITEMS);
}

//单行文本框发送指令数据
void SphereBot::on_sendString_editingFinished()
{
    if(!ui->sendString->text().isEmpty())
    {
        if(!bot->send(ui->sendString->text()))
        {
            qDebug()<<bot->port->errorString();
            sendDataUI("Error while sending Data!");
        }
    }
}

//单行文本框发送指令数据
void SphereBot::on_sendButton_clicked()
{
    on_sendString_editingFinished();
    isSendButton=true;
    ui->sendString->setText("");
    ui->sendButton->setEnabled(false);
}

//滑动条调节舵机角度
void SphereBot::on_servoSlider_sliderMoved(int position)
{
    if(sendState != Sending)
    {
        QString tmp = ("M300 S" + QString::number(position)+"\n");
        bot->send(tmp);
    }
}

//调节框调节舵机角度
void SphereBot::on_servospinBox_valueChanged(int arg1)
{
    if(sendState != Sending)
    {
        ui->servoSlider->setValue(arg1);
        QString tmp = ("M300 S" + QString::number(arg1)+"\n");
        bot->send(tmp);
    }
}

//滑动条调节Y轴位置
void SphereBot::on_penSlider_valueChanged(int value)
{
    if(sendState != Sending)
    {
        ui->penRotationBox->setValue(value);
    }
}

//调节框调节Y轴位置
void SphereBot::on_penRotationBox_valueChanged(int arg1)
{
    if(sendState != Sending)
    {
        QString tmp = ("G1 Y" + QString::number((double)arg1));
        bot->send(tmp);
        ui->penSlider->setValue(arg1);
    }
}

//滑动条调节X轴位置
void SphereBot::on_eggSlider_valueChanged(int value)
{
    if(sendState != Sending)
    {
        ui->eggRotationBox->setValue(value);
    }
}

//调节框调节X轴位置
void SphereBot::on_eggRotationBox_valueChanged(int arg1)
{
    if(sendState != Sending)
    {
        QString tmp = ("G1 X" + QString::number((double)arg1)+"\n");
        bot->send(tmp);
        ui->eggSlider->setValue(arg1);
    }
}

//加载文件到文本框
void SphereBot::on_loadFileButton_clicked()        //== Abort Button
{
    if(sendState == Idle)
    {
        QString fileName;
        if(!curDir.isEmpty())
        {
            fileName = QFileDialog::getOpenFileName(this,tr("Open File"),curDir,tr("GCode Files(*.gcode)"));
        }
        else
        {
            fileName = QFileDialog::getOpenFileName(this,tr("Open File"),tr("."),tr("GCode Files(*.gcode)"));
        }
        if (!fileName.isEmpty())
        {
            loadFile(fileName);
        }
    }
    else
    {
        setState(Idle);
    }
}

//保存文件
void SphereBot::on_saveFileButton_clicked()
{
    saveFile(curFile);
    ui->saveFileButton->setEnabled(false);
}

//加载进来文件
void SphereBot::on_fileTextEdit_textChanged()
{
    if(!ui->fileTextEdit->toPlainText().isEmpty())
    {
        setWindowTitle("Spherebot Controll");
        ui->fileName->setText("");
        if(bot->isConnected())
            ui->sendFileButton->setEnabled(true);
        else
            ui->sendButton->setEnabled(false);
    }
}

//串口连接传送对象
void SphereBot::connectTranceiver()
{
    //connect(this->bot->port,SIGNAL(readyRead()),(&this->transceiver),SLOT(sendNext()));
    connect( this->bot->port, SIGNAL(readyRead()), this, SLOT(receiveData()));
    isCon=true;
}

//串口与数据传输解除关联
void SphereBot::disconnectTranceiver()
{
    //disconnect(this->bot->port,SIGNAL(readyRead()),(&this->transceiver),SLOT(sendNext()));
    disconnect( this->bot->port, SIGNAL(readyRead()), this, SLOT(receiveData()));
    isCon=false;
}

//设置当前设备的状态
void SphereBot::setState(SphereBot::SendStates state)
{
    switch(state)
    {
    case(Idle):
        isSendButton=true;
        switch(sendState)
        {
        case(Sending):      //from Sending to Idle
            //same code as below
            ui->fileSendProgressBar->setValue(0);
        case(Stoped):   //from Stoped to Idle
            disconnectTranceiver();
            this->transceiver.resetState();
            ui->sendFileButton->setText("Send File");
            ui->loadFileButton->setText("Load File");
            ui->restartButton->setEnabled(false);
            ui->controllBox->setEnabled(true);
            ui->fileSendProgressBar->setEnabled(false);
            ui->loadFileButton->setEnabled(true);
            sendState = Idle;
            break;
        default:
            break;
        }
        break;
    case(Sending):
        isSendButton=false;
        switch(sendState)
        {
        case(Idle):     //start sending     //from Idle to Sending
            sendState = Sending;
            if(isCon!=true)
                connectTranceiver();
            ui->controllBox->setEnabled(false);
            ui->fileSendProgressBar->setEnabled(true);
            ui->sendFileButton->setText("Stop");
            ui->restartButton->setEnabled(true);
            ui->loadFileButton->setText("Abort");
            ui->loadFileButton->setEnabled(true);
            ui->sendString->setEnabled(false);
            ui->controllBox->setEnabled(false);
            ui->sendButton->setEnabled(false);
            transceiver.set(ui->fileTextEdit->toPlainText(),(*this->bot));
            transceiver.run();
            statusBar()->showMessage(tr("Sending File"));
            break;
        case(Stoped):   //continue
            sendState = Sending;
            if(isCon!=true)
                connectTranceiver();
#ifdef Watchdog
            transceiver.watchdogTimer->start();
#endif
            this->transceiver.sendNext();
            ui->loadFileButton->setEnabled(false);
            ui->resetButton->setEnabled(true);
            ui->loadFileButton->setText("Abort");
            ui->loadFileButton->setEnabled(true);
            ui->sendFileButton->setText("Stop");
            ui->controllBox->setEnabled(false);
            ui->sendButton->setEnabled(false);
            ui->sendString->setEnabled(false);
            statusBar()->showMessage(tr("Sending File"));
            break;
        default:
            break;
        }
        break;
    case(Stoped):
        isSendButton=true;
        switch(sendState)
        {
        case(Idle):

            break;
        case(Sending):      //from Sending to Stoped
            sendState = Stoped;
            disconnectTranceiver();
            ui->restartButton->setEnabled(true);
            ui->sendFileButton->setText("Continue");
            ui->loadFileButton->setText("Abort");   //used as Abort button
            ui->loadFileButton->setEnabled(true);
            ui->controllBox->setEnabled(true);
            ui->sendButton->setEnabled(true);
            ui->sendString->setEnabled(true);
            statusBar()->showMessage(tr("Stoped sending File"));
            //vScrollBar->setSliderPosition(transceiver.getLineCounter());
            //ui->fileTextEdit->setVerticalScrollBar();
            break;
        default:
            break;
        }
        break;
    }
    qDebug()<<"state: "<<state;
}

//点击发送文件按钮如果是空闲状态则发送数据，如果是发送状态则停止，如果是停止则继续发送
void SphereBot::on_sendFileButton_clicked()
{
    // QScrollBar *vScrollBar = ui->fileTextEdit->verticalScrollBar();
    switch(sendState)
    {
    case 0:         //start to send
        qDebug()<<"start sending";
        setState(Sending);
        break;
    case 1:       //stop the print
        qDebug()<< "You have stoped sending";
        setState(Stoped);
        break;
    case 2:      //continue
        setState(Sending);
        break;
    }
}

//从新发送数据文件
void SphereBot::on_restartButton_clicked()
{
    isSendButton=false;
    sendState = Sending;
    transceiver.set(ui->fileTextEdit->toPlainText(),(*this->bot));
    if(isCon!=true)
        connectTranceiver();
    ui->controllBox->setEnabled(false);
    ui->fileSendProgressBar->setEnabled(true);
    ui->sendFileButton->setText("Stop");
    ui->loadFileButton->setText("Load File");
    ui->controllBox->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->sendString->setEnabled(false);
    ui->loadFileButton->setEnabled(false);
    transceiver.set(ui->fileTextEdit->toPlainText(),(*this->bot));
    transceiver.run();
    statusBar()->showMessage(tr("Sending File"));
}

//发送Y方向的直线运动到0并抬笔命令
void SphereBot::SetBotToHomePosition()
{
    QString tmp = ("G1 Y0");
    tmp.append("\nM300 S" + QString::number(penUpAngle));
    qDebug()<<"to print: "<<tmp;
    bot->send(tmp);
}

//改变设备速度
void SphereBot::on_servoFeedrateSlider_valueChanged(int value)
{
    if(sendState != Sending)
    {
        QString tmp = ("G1 F" + QString::number(value)+"\n");
        bot->send(tmp);
    }
}

//调节球状物直径
void SphereBot::on_setDiameterButton_clicked()
{
    if(sendState != Sending)
    {
        QString tmp = ("M400 S" + QString::number(ui->diameterSlider->value())+"\n");
        bot->send(tmp);
        tmp.clear();
        tmp = ("M401 S" + QString::number(ui->diameterSlider->value())+"\n");
        bot->send(tmp);
    }
}

//撤销
void SphereBot::on_undoButton_clicked()
{
    ui->fileTextEdit->undo();
}

//重做
void SphereBot::on_redoButton_clicked()
{
    ui->fileTextEdit->redo();
}

//文本可以撤销时的处理
void SphereBot::on_fileTextEdit_undoAvailable(bool b)
{
    ui->saveFileButton->setEnabled(true);
    if(b)
        ui->undoButton->setEnabled(true);
    else
        ui->undoButton->setEnabled(false);
}

//文本可以重做时的处理
void SphereBot::on_fileTextEdit_redoAvailable(bool b)
{
    if(b)
        ui->redoButton->setEnabled(true);
    else
        ui->redoButton->setEnabled(false);
}

//发送单行文本框内容改变时的处理
void SphereBot::on_sendString_textChanged(const QString &arg1)
{
    if(!arg1.isEmpty())
    {
        ui->sendButton->setEnabled(true);
    }
    else
    {
        ui->sendButton->setEnabled(false);
    }
}

//在文件中查找参数
int getOption(QString string,QString searchString)
{
    int index = string.indexOf(searchString);//从字符串的开头向后搜索子串
    if(index != -1)
    {
        int i = index;
        while(string.at(i) != ' ')
            i++;
        qDebug()<<"index: "<<index+searchString.length()<<"i"<<i;
        QString subsection = string.mid(index+searchString.length(),(i - (index+searchString.length())));
        qDebug()<<subsection;
        return subsection.toInt();
    }
    return -1;
}
