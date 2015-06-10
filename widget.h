#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QSettings>
#include <vector>

#include "graphicsitems.h"









namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget          *ui;
    CustomScene         *scene;   
};





#endif // WIDGET_H
