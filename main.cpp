#include "mainwindow.h"

#include <QApplication>
#include <QTableWidget>

//#include <QSerialPort>
//#include <QSerialPortInfo>

int main(int argc, char *argv[])  //ceparate every class
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show(); 
    return a.exec();
}
