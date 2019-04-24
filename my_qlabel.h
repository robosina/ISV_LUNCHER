#ifndef MY_QLABEL_H
#define MY_QLABEL_H
#include <QLabel>
#include <QMouseEvent>
#include <QDebug>
#include <QEvent>


class my_qlabel : public QLabel
{
    Q_OBJECT
public:
    explicit my_qlabel(QWidget *parent=0);
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

signals:
    void mouseDouble();
    void mousePressed();
    void mouseReleased();

};

#endif // MY_QLABEL_H
