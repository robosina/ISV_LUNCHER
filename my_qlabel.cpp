#include "my_qlabel.h"

my_qlabel::my_qlabel(QWidget *parent):
    QLabel(parent)
{

}


void my_qlabel::mouseDoubleClickEvent(QMouseEvent *ev)
{
    emit mouseDouble();
}

void my_qlabel::mousePressEvent(QMouseEvent *ev)
{
    emit mouseDouble();
}
