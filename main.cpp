#include "server.h"
#include "clinet.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server w;
    w.show();
    Clinet c;
    c.show();


    return a.exec();
}
