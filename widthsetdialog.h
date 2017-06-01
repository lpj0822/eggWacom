#ifndef WIDTHSETDIALOG_H
#define WIDTHSETDIALOG_H

#include <QDialog>
#include<QSpinBox>
#include<QHBoxLayout>
#include<QPushButton>
#include<QRect>
#include"datadef.h"

class WidthSetDialog : public QDialog
{
    Q_OBJECT

public:

    explicit WidthSetDialog(int defaultValue=WIDTHWEIGHT, QWidget *parent = 0);

    ~WidthSetDialog();

    int getValue() {return initWidth; }

signals:

private slots:
    void okSlot();

private:
    QHBoxLayout *mainlayout;
    QSpinBox *widthSpinBox;
    QPushButton *okButton;

    int initWidth;
};

#endif // WIDTHSETDIALOG_H
