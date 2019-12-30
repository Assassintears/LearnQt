#include "maindow.h"
#include "cdata.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidow w;
    w.show();

    return a.exec();
}
