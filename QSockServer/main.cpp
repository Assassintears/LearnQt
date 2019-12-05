#include "qsockserver.h"
#include "quedata.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSockServer w;
    w.show();

    return a.exec();
}
