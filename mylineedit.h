#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H
#include <QLineEdit>
#include <QObject>
#include <QFocusEvent>
#include <QEvent>
class mylineedit : public QLineEdit
{
    Q_OBJECT
public:
    explicit mylineedit(QWidget *parent = 0);
    void focusInEvent(QFocusEvent *e);
    void focusOutEvent(QFocusEvent *e);
signals:
    void focusin();
    void focusout();
};

#endif
