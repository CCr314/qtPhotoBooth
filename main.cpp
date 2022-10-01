#include "mainwindow.h"
#include <QApplication>
#include "params.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    params::init();
    MainWindow w;
    w.show();

    return a.exec();
}
