#include "widthsetdialog.h"

WidthSetDialog::WidthSetDialog(int defaultValue, QWidget *parent):
    QDialog(parent),initWidth(defaultValue)
{
   widthSpinBox=new QSpinBox();
   widthSpinBox->setRange(0, 100);
   widthSpinBox->setSingleStep(1.0);
   widthSpinBox->setValue(defaultValue);

   okButton=new QPushButton(tr("确定"));
   connect(okButton,SIGNAL(clicked()),this,SLOT(okSlot()));

   mainlayout=new QHBoxLayout();
   mainlayout->addWidget(widthSpinBox);
   mainlayout->addWidget(okButton);
   setLayout(mainlayout);

   //this->setWindowModality(Qt::NonModal);
   this->setModal(true);
   this->setMinimumSize(153,45);
   this->setMaximumSize(153,45);
   this->setWindowTitle(tr("Width"));
}

WidthSetDialog::~WidthSetDialog()
{

}

void WidthSetDialog::okSlot()
{
    initWidth=this->widthSpinBox->value();
    accept();
}
