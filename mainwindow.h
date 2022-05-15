#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QTimer>
#include <QTime>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ButtonStart_clicked();
    void readData();

    void on_ButtonStop_clicked();

  private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort;
    //QTimer *timer;

};
#endif // MAINWINDOW_H
