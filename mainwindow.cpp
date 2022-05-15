#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QTime>

int start_stop=1;


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->ButtonStart->setStyleSheet("QPushButton{border: 1px solid transparent;text-align: center;"          //классные кнопы
                                      "color:rgba(255,255,255,255);"
                                      "border-radius: 8px;"
                                      "border-width: 3px;"
                                      "border-image: 9,2,5,2; "
                                      "background-position: top left;"
                                      "background-origin: content;"
                                      "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(92, 191, 78, 1), stop:1 rgba(92, 191, 78, 1));}"
                                      "QPushButton::chunk {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(38,148,24,1), stop: 1 rgba(38,148,24,1));}"
                                      "QPushButton{border-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(45, 168, 29, 1), stop:1 rgba(45, 168, 29, 1));}"
                                      );
  ui->ButtonStop->setStyleSheet("QPushButton{border: 1px solid transparent;text-align: center;"
                                      "color:rgba(255,255,255,255);"
                                      "border-radius: 8px;"
                                      "border-width: 3px;"
                                      "border-image: 9,2,5,2; "
                                      "background-position: top left;"
                                      "background-origin: content;"
                                      "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(235, 59, 59, 1), stop:1 rgba(235, 59, 59, 1));}"
                                      "QPushButton::chunk {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(255,200,0,255), stop: 1 rgba(255,0,0,255));}"
                                      "QPushButton{border-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(171, 21, 21, 1), stop:1 rgba(171, 21, 21, 1));}"
                                      );

// setStyleSheet("QMainWindow{background-color: black; border: 1px solid black;}");    //pretty? black theme
// ui->label->setStyleSheet("color: rgb(250, 250, 250)");
// ui->label_7->setStyleSheet("color: rgb(250, 250, 250)");
// ui->label_2->setStyleSheet("color: rgb(250, 250, 250)");
// ui->label_4->setStyleSheet("color: rgb(250, 250, 250)");
// ui->label_5->setStyleSheet("color: rgb(250, 250, 250)");
// ui->label_6->setStyleSheet("color: rgb(250, 250, 250)");
// ui->labelHeader->setStyleSheet("color: rgb(250, 250, 250)");
// ui->labelLatitude->setStyleSheet("color: rgb(250, 250, 250)");
// ui->labelLongitude->setStyleSheet("color: rgb(250, 250, 250)");
// ui->labelHeight->setStyleSheet("color: rgb(250, 250, 250)");
// ui->label_9->setStyleSheet("color: rgb(250, 250, 250)");
// ui->label_10->setStyleSheet("color: rgb(250, 250, 250)");
// ui->label_11->setStyleSheet("color: rgb(250, 250, 250)");
// ui->label_12->setStyleSheet("color: rgb(250, 250, 250)");
// ui->label_14->setStyleSheet("color: rgb(250, 250, 250)");
// ui->labelPreSum->setStyleSheet("color: rgb(250, 250, 250)");
// ui->labelPastSum->setStyleSheet("color: rgb(250, 250, 250)");


  foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())     //автодетекция COM-портов
  ui->comboBox->addItem(info.portName());

  serialPort = new QSerialPort(this);

  if (serialPort->portName() != ui->comboBox->currentText())   //установка снятия данных на найденный (первый) порт
  {
    serialPort->close();
    serialPort->setPortName(ui->comboBox->currentText());
  }

  serialPort->open(QIODevice::ReadOnly);
  serialPort->setBaudRate(QSerialPort::Baud115200);

  if (serialPort->isOpen())
  qDebug() << "Connected";
  else
  {
  qDebug() << "Open error";
  ui->textFull->append("Open error (скорее всего порт занят или устройство не подключено)");   //также будет возвращаться если настроек нет а порт открыт
  }

  connect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));

  ui->tableGSV->setColumnCount(6);                   //Устанавливает кол-во столбцов равным пяти
  ui->tableGSV->setHorizontalHeaderLabels(QStringList() << "Type" << "∑sat." << "ID" << "Tngl" << "Az"<< "SNR");
  ui->tableGSV->verticalHeader()->setVisible(false);

}

MainWindow::~MainWindow()
{
  delete ui;
  //serialPort ->close();
  //delete serialPort;
}

void MainWindow::on_ButtonStart_clicked()
{
  start_stop=1;  //возврат от остановки приёма данных

//  /*
//  timer = new QTimer();
//  connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
//  timer->start(1); // И запустим таймер
//  */

}

void MainWindow::readData(){
  if (start_stop!=0){  //проверка не нажата ли кнопка "стоп"
  QString division =",";
  if (serialPort->portName() != ui->comboBox->currentText())   //повторная установка снимаемого порта если пользователь сменил порт вручную
  {
    serialPort->close();
    serialPort->setPortName(ui->comboBox->currentText());
  }

  if (!serialPort->isOpen())   //ошибка подключения
  {
    qDebug() << "not open the port?";
  }

  QByteArray data = serialPort->readAll();
  QString dataAAAstring = serialPort->readAll();

  ui->textFull->append(data);

  QString dataString = QString::fromStdString(data.toStdString());   //надежный перевод из bytearray в string
  //qDebug() << "Кол-во символов (ASCII): " << dataString.length();    //оч плохо когда <46 - какие-то данные потерялись

  // ----------------------------------- поиск GGA ----------------------------------------
  int startGGAPos = dataString.indexOf("GGA");
  int endGGAPos = dataString.indexOf("*", startGGAPos);
  QString dataStringGGA = dataString.mid(startGGAPos-2, endGGAPos - startGGAPos+2);

  if(startGGAPos>1 && endGGAPos>1)           //если GGA норм пришло
  {
    int position[14];                            //здесь присваиваем каждой в будущем найденной запятой отрицательный номер(позицию в сообщении)- для обнаружения ошибок
    for (int numPos=0; numPos<13; numPos++)\
    {
      position[numPos]=-1;
    }
    int numPos=0;     //number of position - для перебора запятых
    for(int i=0; i<dataStringGGA.length(); i++)     //здесь присваиваем каждой запятой свой номер
      if(dataStringGGA[i]==division)
      {
       if (numPos<13){                   //всего запятых 14 так что проверяем не словили ли лишнюю откуда-то
          position[numPos]=i;
          numPos++;
        }
        else
        {
         break;
        }
      }

    qDebug() << "GGA data was found";
    QString dataHeader = dataStringGGA.mid(position[0]-3,3);   //вывод типа сообщения (сейчас это GGA)
    ui->labelHeader->setText(dataHeader);

//    QString startSymbol ="$";     //а это уже для чексуммы
//    QString endSymbol ="*";
//    for(int i=0; i<dataStringGGA.length(); i++)         //находим символы $ и *
//      {
//      if(dataStringGGA[i]==startSymbol)
//        {
//        posStart=i;
//        }
//      else if (dataStringGGA[i]==endSymbol && posStart>1){
//        posEnd=i;
//        }
//      }
//    if(posEnd-posStart>0)
//    {

      //int lengthNmea=dataStringGGA.length();                                  //выделяем сообщение между $ и *
      //QString stringNMEA = dataStringGGA.mid(posStart+1,lengthNmea);

      ui->textNMEAGGA->append(dataStringGGA);

      int sumXOR=00;         //переменная для нахождения XOR всех бит данных

      for (int i=0; i<dataStringGGA.length(); i++)     //превращаем в поток байтов (пробовал много вариантов но только схема string->hex->binary->десятичные->string работает)))
        {
        QString nextSymbol = dataStringGGA[i];
        QString hexadecimalNumber = nextSymbol.toLocal8Bit().toHex();    //рабочий конвертер в hex из потока символов

        bool ok = false;
        QString binaryNumber = QString::number(hexadecimalNumber.toLongLong(&ok, 16), 2);    //рабочий конвертер в binary form из hex

        while (binaryNumber.length()<8)            //обрезает первые нули так что докидываем их если отсутствуют
          {
          binaryNumber = "0" + binaryNumber;
          }
        //ui->textBinary->append(binaryNumber);
        QString dec_str;                           //перевод из двоичной в десятичную
        bool ok2;
        int dec = binaryNumber.toInt(&ok2,2);
        if (ok2)
           dec_str.setNum(dec);

        QTextStream stream(&dec_str);    //перевод из string в int !!! стырил откуда-то

        QList<int> array;
        while (!stream.atEnd())
          {
           int number;
           stream >> number;
           array.append(number);
          }

        sumXOR ^= array[0];
        }

      //QString stringDecSumXOR = QString::number(sumXOR);     //int в string
      QString XORresult = QString::number( sumXOR, 16 ).toUpper();         //в hex из десятичной, также делает string
      QString pastSum = dataString.mid(endGGAPos+1,2);        //чек контрольных битов после данных
      //----далее для отображения контрольных байт в binary формате-----
//      bool ok = false;
//      QString binaryPastSum = QString::number(pastSum.toLongLong(&ok, 16), 2);  //в binary из hex - контрольная сумма по последним 2м байтам
//      while (binaryPastSum.length()<8)            //обрезает первые нули так что докидываем их если отсутствуют
//        binaryPastSum = "0" + binaryPastSum;
//      ui->labelPastSum->setText(binaryPastSum);

      if (pastSum==XORresult)      //проверка равны ли контрольные суммы
          ui->checkBox->setChecked(1);
      else
        ui->checkBox->setChecked(0);

      if(position[2]-position[1]>1)         //проверяем на наличие данных о широте и их вывод
      {
        int dataLength = position[2]-position[1];
        QString dataLatitude = dataStringGGA.mid(position[1]+1,dataLength-1+2);
        ui->labelLatitude->setText(dataLatitude);

        QString dataHumanLatitude = dataLatitude.mid(0, 2) + "°" + dataLatitude.mid(2, dataLatitude.length()-4) + "'" + dataLatitude.mid(dataLatitude.length()-1, 1);
        ui->lineLatitude->setText(dataHumanLatitude);           //представление в человечесском виде
      }
      else
      {
        ui->labelLatitude->setText("no data");
      }
      if(position[4]-position[3]>1)         //проверяем на наличие данных о долготе и их вывод
      {
       int dataLength = position[4]-position[3];
       QString dataLongitude = dataStringGGA.mid(position[3]+1,dataLength-1+2);
       ui->labelLongitude->setText(dataLongitude);

        QString dataHumanLongitude = dataLongitude.mid(1, 2) + "°" + dataLongitude.mid(3, dataLongitude.length()-5) + "'" + dataLongitude.mid(dataLongitude.length()-1, 1);
        ui->lineLongitude->setText(dataHumanLongitude);         //представление в человечесском виде
      }
      else
      {
        ui->labelLongitude->setText("no data");
      }
      if(position[9]-position[8]>1)        //проверяем на наличие данных о высоте (относительно уровня моря) и их вывод
      {
        int dataLength = position[9]-position[8];
        QString dataHeight = dataStringGGA.mid(position[8]+1,dataLength-1+2);
        ui->labelHeight->setText(dataHeight);
      }
      else
      {
        ui->labelHeight->setText("no data");
      }
  }
  else            //если GGA не попалось
  {
  qDebug() << "GGA data not found";
  ui->checkBox->setChecked(0);
  }

  //----------------------------------------- NTLab binary (не доделал) (вообще не то делал) -------------------------------------
//  QString binaryNumber;              //каждый символ
//  QString allBinaryFlow ;         //весь пакет в битовом виде
//  QString msgBinary;             //каждый пакет данных в общем одном сообщении
//  QString checkSum;
//  int sumXOR=00;         //переменная для нахождения XOR всех бит данных

//  for (int i=0; i<dataString.length(); i++)     //превращаем в поток байтов (схема string->hex->binary)          //ненужная часть - это не настоящий binary
//    {
//    QString nextSymbol = dataString[i];
//    QString hexadecimalNumber = nextSymbol.toLocal8Bit().toHex();    //в hex из потока символов

//    bool ok = false;
//    binaryNumber = QString::number(hexadecimalNumber.toLongLong(&ok, 16), 2);    //в binary form из hex

//    while (binaryNumber.length()<8)            //обрезает первые нули так что докидываем их если отсутствуют
//      {
//      binaryNumber = "0" + binaryNumber;
//      }
//    allBinaryFlow = allBinaryFlow + binaryNumber;
//    }

//  int startSync = allBinaryFlow.indexOf("0010000101001110");    //detect 2 bytes of synchronization
//  int msgEnd=-1;

//  for (int i=0; i<allBinaryFlow.length(); i++)                  //цикл нужен для обнаружения нескольких пакетов за одно сообщение
//    {
//    if(startSync>msgEnd)                                          //если нашелся пакет новый, чтобы не повторялось
//    {
//    int msgStart = allBinaryFlow.indexOf("00100100", startSync+1);  //$
//    msgEnd = allBinaryFlow.indexOf("00101010", msgStart);       //*
//    msgBinary = allBinaryFlow.mid(msgStart+8, msgEnd-msgStart-8);   //ловим каждое между $ и * (Obtain the data length field and check it for the maximum value)

//    //проверка на нашлись ли нью данные
//    if (msgBinary.length()<4104*8 && msgBinary.length()>8*8 && msgEnd>0)  //если сообщение норм длины данных & есть вообще конец сообщения & есть биты синхр для сообщ
//      {
//      checkSum = allBinaryFlow.mid(msgEnd+8, 16);     //obtain two bytes of checksum - в binary формате
//      //QString qmessage = QString::fromStdString(checkSum);

//      //ОСТАЛОСЬ ТОЛЬКО ПРЕОБРАЗОВАТЬ ИЗ BINARY ОБРАТНО В ASCII И ПОЛУЧИТСЯ HEX-ОВОЕ И ГОТОВО ---------

////      qDebug()<< checkSum ;
////      bool ok = false;
////      QString binaryNumber = QString::number(checkSum.toLongLong(&ok, 2), 16);
////      qDebug()<< binaryNumber ;
//     // QString qmessage = QString::fromStdString(checkSum);
//      //переводим назад в hex

//      for (int i=0; i<msgBinary.length(); i+=8)  //нарезка на символы и нахождение XOR
//      {
//        QString oneSymbol = msgBinary.mid(i, 8);
//        QString dec_str2;                           //перевод из двоичной в десятичную
//        bool ok3;
//        int decMsg = oneSymbol.toInt(&ok3,2);  //косяк
//        if (ok3)
//           dec_str2.setNum(decMsg);

//        QTextStream stream(&dec_str2);    //перевод из string в int !!! стырил откуда-то
//        QList<int> array;
//        while (!stream.atEnd())
//          {
//            int number;
//            stream >> number;
//            array.append(number);
//          }
//        sumXOR ^= array[0];
//      }

//      QString XORresult = QString::number( sumXOR, 16 ).toUpper();         //в hex из десятичной, также делает string

//      if(XORresult==checkSum)
//        ui->textBinary->append("Пакет " "*здесь должно быть сообщение типо GLGGA*" "без ошибок");
//      else
//        ui->textBinary->append("Пакет " "*здесь должно быть сообщение типо GLGGA*" "с ошибкой" "(но на самом деле контрольная сумма считается верно)))");
//      }

//    startSync = allBinaryFlow.indexOf("0010000101001110", msgEnd);      //для детекции новго пакета в одном сообщении
//    }
//    }           //конец перебора битового всего сообщения

//    ui->textBinary->append("------ END MSG -------");

  // ------------------------------------ детекция GSV ------------------------------------------

  int startGSVPos = dataString.indexOf("GSV");
  int endGSVPos =0;
  int counterOfData =0;        //строка в таблице = номер спутника по счёту из рассматриваемых
  QString numberOfPackage;
    while (startGSVPos>endGSVPos)
      {
      counterOfData++;
      endGSVPos = dataString.indexOf("*", startGSVPos);
      QString dataStringGSV = dataString.mid(startGSVPos-2, endGSVPos - startGSVPos+2);

      startGSVPos = dataString.indexOf("GSV", endGSVPos);    //нашли следующий пакет GSV

      ui->tableGSV->setRowCount(counterOfData);

      int position[34];                            //присваиваем каждой в будущем найденной запятой отрицательный номер(позицию в сообщении) (взял кол-во с запасом)
      for (int numPos=0; numPos<33; numPos++)
      {
        position[numPos]=-1;
      }

      int numPos=0;                                    //number of position - для перебора запятых
      for(int i=0; i<dataStringGSV.length(); i++)     //здесь присваиваем каждой запятой свой номер
        if(dataStringGSV[i]==division)
        {
          position[numPos]=i;
          numPos++;
        }

      if(numberOfPackage!=dataStringGSV.mid(position[0]-5, position[1]-position[0]+5))       //прост для красоты отображения
      {
        ui->tableGSV->setItem(counterOfData-1, 0, new QTableWidgetItem(dataStringGSV.mid(0, 5)));     //xxGSV отображение
        if (position[2]>0)
          ui->tableGSV->setItem(counterOfData-1, 1, new QTableWidgetItem(dataStringGSV.mid(position[2]+1, position[3]-position[2]-1 )));  //number of satellites
        //numberOfPackage=dataStringGSV.mid(position[0]+1, position[1]-position[0]-1);     //рабочий вар
        numberOfPackage=dataStringGSV.mid(position[0]-5, position[1]-position[0]+5);
      }

      int commaNumber=4;                    //прост некст запятая - так удобнее было
      while(position[commaNumber]>0)      //ну тут прост проверяем дошли ли остальные данные и сколько их
        {
        if (position[commaNumber]>0)    //если пришло ...
          ui->tableGSV->setItem(counterOfData-1, 2, new QTableWidgetItem(dataStringGSV.mid(position[commaNumber-1]+1, position[commaNumber]-position[commaNumber-1]-1 )));  //ID
        commaNumber++;
        if (position[commaNumber]>0)
          ui->tableGSV->setItem(counterOfData-1, 3, new QTableWidgetItem(dataStringGSV.mid(position[commaNumber-1]+1, position[commaNumber]-position[commaNumber-1]-1 )));  //Tngl
        commaNumber++;
        if (position[commaNumber]>0)
          ui->tableGSV->setItem(counterOfData-1, 4, new QTableWidgetItem(dataStringGSV.mid(position[commaNumber-1]+1, position[commaNumber]-position[commaNumber-1]-1 )));  //Az
        commaNumber++;
        if (position[commaNumber-1]>0)    //а тут ещё -1 ибо последней запятой нет для вырезки содержимого и вырезает прост до конца
          ui->tableGSV->setItem(counterOfData-1, 5, new QTableWidgetItem(dataStringGSV.mid(position[commaNumber-1]+1, position[commaNumber]-position[commaNumber-1]-1 )));   //SNR
        commaNumber++;
        if(position[commaNumber]>0)          //если запятые в отдельном пакете всё ещё есть
          counterOfData++;

        ui->tableGSV->setRowCount(counterOfData);
        }
      }                    //конец проверки на наличие GSV сообщения

   ui->tableGSV->resizeColumnsToContents();
   ui->tableGSV->resizeRowsToContents();
   if(endGSVPos == 0)
     qDebug() << "GSV data not found";
   else
     qDebug() << "GSV data was found";


  qDebug() << "-------  end reception  --------";

  }        //конец проверки на нажатую кнопку "стоп"
}

void MainWindow::on_ButtonStop_clicked()
{
    start_stop=0;  //остановка приёма данных    //лучше вообще закрывать порт
}
