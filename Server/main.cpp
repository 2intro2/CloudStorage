#include "server.h"
#include "operatordb.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OperatorDB::getInstance().connectDB();
    server::getInstance();
    return a.exec();
}
