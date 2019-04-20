#include "mylineedit.h"

mylineedit::mylineedit(QWidget *parent)
    : QLineEdit(parent)
{

}


void mylineedit::focusInEvent(QFocusEvent *e)
{
    emit focusin();
}

void mylineedit::focusOutEvent(QFocusEvent *e)
{
    emit focusout();
}
