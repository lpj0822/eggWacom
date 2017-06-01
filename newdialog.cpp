#include "newdialog.h"

NewDialog::NewDialog(int width,int height,QWidget* parent):QDialog(parent)
{
    this->canvas_width=width;
    this->canvas_height=height;
    initUI();
    this->setModal(true);
    this->setMinimumSize(153,135);
    this->setMaximumSize(153,135);
    this->setWindowTitle(tr("New canvas"));
}

NewDialog::~NewDialog()
{

}

void NewDialog::initUI()//初始化UI
{
    QHBoxLayout *layout=new QHBoxLayout();
    wLabel=new QLabel(tr("Width:"));
    widthSpinBox=new QSpinBox();
    widthSpinBox->setRange(1, 10000);
    widthSpinBox->setSingleStep(1);
    widthSpinBox->setValue(canvas_width);
    layout->addWidget(wLabel);
    layout->addWidget(widthSpinBox);

    QHBoxLayout *layout1=new QHBoxLayout();
    hLabel=new QLabel(tr("Height:"));
    heightSpinBox=new QSpinBox();
    heightSpinBox->setRange(1,10000);
    heightSpinBox->setSingleStep(1);
    heightSpinBox->setValue(canvas_height);
    layout1->addWidget(hLabel);
    layout1->addWidget(heightSpinBox);

    QHBoxLayout *layout2=new QHBoxLayout();
    okButton=new QPushButton(tr("确定"));
    noButton=new QPushButton(tr("取消"));
    layout2->addWidget(okButton);
    layout2->addWidget(noButton);

    mainlayout=new QVBoxLayout();
    mainlayout->addLayout(layout);
    mainlayout->addLayout(layout1);
    mainlayout->addLayout(layout2);
    this->setLayout(mainlayout);

    connect(okButton,SIGNAL(clicked()),this,SLOT(okSlot()));
    connect(noButton,SIGNAL(clicked()),this,SLOT(noSlot()));
}

void NewDialog::okSlot()
{
    canvas_width=this->widthSpinBox->value();
    canvas_height=this->heightSpinBox->value();
    accept();
}

void NewDialog::noSlot()
{
  close();
}

int NewDialog::getWidth()
{
    return canvas_width;
}

int NewDialog::getHeight()
{
    return canvas_height;
}
