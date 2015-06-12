#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    scene = new CustomScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
#ifdef Q_OS_WIN
#else
    ui->graphicsView->rotate(180);
#endif
    scene->setSceneRect(0,0,800,480);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    qDebug() << "Main Thread ID:  " << QThread::currentThreadId();

    ui->graphicsView->show();
}

Widget::~Widget()
{
    // delete
    delete scene;
    delete ui;
}
