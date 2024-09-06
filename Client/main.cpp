#include "client.h"
#include "index.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Client::getInstance().show();

    return a.exec();
}
