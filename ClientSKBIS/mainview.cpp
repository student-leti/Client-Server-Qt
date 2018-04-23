#include "mainview.h"
#include "ui_mainview.h"
#include "message.h"
#include <QtCore/QDebug>
#include <QTcpSocket>
#include <QMessageBox>
#include <QString>
#include <QByteArray>



MainView::MainView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainView),
    m_socket(new QTcpSocket)
{
    ui->setupUi(this);
    ui->btnConnect->setEnabled(true);
    ui->btnDisconnect->setEnabled(false);
    ui->btnSend->setEnabled(false);
    connect(m_socket, &QTcpSocket::disconnected, this,
            &MainView::onDisconnect);
    connect(m_socket, &QTcpSocket::readyRead, this,
            &MainView::readyRead);
    connect(m_socket, &QTcpSocket::connected, this,
            &MainView::onConnected);
}

MainView::~MainView()
{
    delete ui;
}

void MainView::on_btnConnect_clicked()
{
    SetConnection();
    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(true);
    ui->btnSend->setEnabled(true);
    ui->editMessage->clear();
    ui->editOpenNoise->clear();
    ui->editCloseNoise->clear();
    ui->editDataParameters->clear();
}

void MainView::on_btnDisconnect_clicked()
{
    onDisconnect();
    ui->btnDisconnect->setEnabled(false);
    ui->btnConnect->setEnabled(true);
    ui->btnSend->setEnabled(false);
}

void MainView::on_btnSend_clicked()
{
    QString to_send = CollectMessage();
    m_socket->write(to_send.toLocal8Bit());//.toLatin1()
    qDebug()<<to_send;
}

void MainView::SetConnection()
{
    m_socket->connectToHost("localhost", 1234, QTcpSocket::ReadWrite);
}

void MainView::onConnected()
{
    ui->editServerResponse->append("Connected!");
    qDebug() << "Connected!";

}

void MainView::onDisconnect()
{
    if(m_socket->isOpen())
    {
        m_socket->disconnectFromHost();
        ui->editServerResponse->append("Disconnected!");
    }
}

void MainView::readyRead()
{
    QByteArray strByte = m_socket->readAll();
    ui->editServerResponse->append(strByte.toHex(':').toUpper());//append(str.toHex(':').toUpper());
}

int MainView::GetOpenNoiseParameter()
{
    QString message = ui->editOpenNoise->toPlainText();
    bool ok;
    short length = message.toShort(&ok, 10);
    return length;
}

int MainView::GetMessageParameter()
{
    QString message = ui->editDataParameters->toPlainText();
    bool ok;
    short length = message.toShort(&ok, 10);
    return length;
}

int MainView::GetMessageLenght()
{
    QString message = ui->editMessage->toPlainText();
    int length = message.size();
    return length;
}

int MainView::GetCloseNoiseParameter()
{
    QString message = ui->editCloseNoise->toPlainText();
    bool ok;
    short length = message.toShort(&ok, 10);
    return length;
}

bool MainView::CompareLegth()
{
    if (GetMessageParameter()>=GetMessageLenght())
    {
        if(GetMessageLenght()+GetCloseNoiseParameter()+GetOpenNoiseParameter()<256)
        {
            return true;
        }
        else
        {
            QMessageBox::critical(this,tr("Sender"),
                                  tr("The message length will be more than 256 bytes!"
                                      "Please, make a correction of the parameters."));
            return false;
        }
    }
    else
    {
        return false;
    }
}

QString MainView::GetMessage()
{
    QString str = ui->editMessage->toPlainText();
    str = "\n" + str + "\v";
    return str;
}

QString MainView::SaltFunction()
{
    int openNoiseParameter = GetOpenNoiseParameter();
    QString openString;
    if(openNoiseParameter>=0)
    {   QChar openChar[openNoiseParameter];
        for(int i=0; i<openNoiseParameter; i++)
            {
              int salt = qrand()%127;
              if (salt!=0xA)
              {
                  openChar[i]=salt;
                  openString=openString.append(openChar[i]);
              }
              else
              {
                  i--;
              }

            }
    }
    return openString;
}

QString MainView::PepperFunction()
{
    int closeNoiseParameter = GetCloseNoiseParameter();
    QString closeString;
    if(closeNoiseParameter>=0)
    {   QChar closeChar[closeNoiseParameter];
        for(int j=0; j<closeNoiseParameter; j++)
        {
          int pepper = qrand()%127;
          if (pepper!=0xB)
          {
              closeChar[j]=pepper;
              closeString=closeString.append(closeChar[j]);
          }
          else
          {
              j--;
          }

        }
    }
    return closeString;
}

QString MainView::CollectMessage()
{
    QString outgoing;
    if(CompareLegth())
    {
        QString salt = SaltFunction();
        QString pepper = PepperFunction();
        QString str = GetMessage();
        outgoing = salt.append(str.append(pepper));
     }
    else
    {
        QMessageBox::critical(this,tr("Sender"),
                              tr("The message is longer than seted parameter!"
                                 "Please, correct the message."));
    }
    qDebug()<<outgoing;
    return outgoing;
}
