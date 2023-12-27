#include "widget.h"

#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QApplication::setQuitOnLastWindowClosed(false);
//    QWidget *widget = new QWidget();
    Widget *w = new Widget;
    w->show();
    w->activateWindow();
    int c = a.exec();
//    delete widget;
    return c;
}
