#include "widget.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.setWindowFlags(Qt::FramelessWindowHint);
#ifdef Q_OS_WIN
#else
    w.setCursor(Qt::BlankCursor);
#endif
    w.show();

    return a.exec();
}
