#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include<QLabel>
#include<QSpinBox>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QPushButton>
#include"datadef.h"

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDialog(int width=WINDOWWIDTH,int height=WINDOWHEIGHT,QWidget *parent = 0);
    ~NewDialog();

    int getWidth();
    int getHeight();

signals:

private slots:
    void okSlot();
    void noSlot();

private:
    void initUI();//初始化UI
    QVBoxLayout *mainlayout;
    QLabel *wLabel;
    QSpinBox *widthSpinBox;
    QLabel *hLabel;
    QSpinBox *heightSpinBox;
    QPushButton *okButton;
    QPushButton *noButton;

    int canvas_width;
    int canvas_height;
};

#endif // NEWDIALOG_H
