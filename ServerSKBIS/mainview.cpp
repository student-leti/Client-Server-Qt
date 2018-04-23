#include "mainview.h"
#include "ui_mainview.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QBitArray>
#include <QByteArray>

static int PORT_NUMBER =1234;
static int WAIT_FOR_DATA_MS = 100;
QBitArray RESULT_ARRAY(8);

MainView::MainView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainView),
    m_server(new QTcpServer(this)),
    m_socket(nullptr)
{
    ui->setupUi(this);
    ui->btnStartServer->setEnabled(true);
    ui->btnStopServer->setEnabled(false);
    connect(m_server, &QTcpServer::newConnection,
            this, &MainView::ExchangeData);
}

MainView::~MainView()
{
    delete ui;
}

void MainView::on_btnStartServer_clicked()
{
    if (StartServer())
    {
        ui->btnStartServer->setEnabled(false);
        ui->btnStopServer->setEnabled(true);
    }

}

void MainView::on_btnStopServer_clicked()
{
    StopServer();
    ui->btnStartServer->setEnabled(true);
    ui->btnStopServer->setEnabled(false);
}

bool MainView::StartServer()
{
    bool result = m_server->listen(QHostAddress::Any, PORT_NUMBER);
    if (!result)
    {
        QMessageBox::critical(this, tr("Echo Server"),
                              tr("Unable to start the server: %1")
                              .arg(m_server->errorString()));
        return false;
    }
    return true;
}

void MainView::StopServer()
{
    m_server->close();
    if ((m_socket !=nullptr)&& m_socket->isOpen())
    {
        m_socket->close();
    }
}

void MainView::ExchangeData()
{
    m_socket = m_server->nextPendingConnection();
    if (m_socket->isOpen())
    {
        connect(m_socket, &QTcpSocket::readyRead,
                this, &MainView::EchoReadData);
    }
}
//read the data and send back a summ of bytes
void MainView::EchoReadData()
{
    QByteArray tmp, echoArray;
    while (!m_socket->atEnd())
    {
        tmp = m_socket->readAll();
        qDebug()<<"start";
        qDebug()<<tmp.toHex('*').toUpper();
        m_socket->waitForReadyRead(WAIT_FOR_DATA_MS);
    }
    echoArray = getAnswer(tmp);
    if(echoArray.size()>0)
    {
        qDebug()<<echoArray;
        m_socket->write(echoArray);
    }
    else if (echoArray.size()==0)
    {
        m_socket->write(echoArray);
    }
    qDebug()<<"end";
}
/*gets incoming message, pass it farther
gets ByteArray to send back*/
QByteArray MainView::getAnswer(const QByteArray& temp)
{
    QByteArray clearMessage, answer;
    clearMessage = getClearPackage(temp);
    if(clearMessage.size()==1)
    {
        answer = clearMessage;        
    }
    else if(clearMessage.size()>1)
    {
        returnBitArray(clearMessage);
        answer = returnByteArray();        
    }
    return answer;
}

/*Finds an opening and closing bytes,return a clear message*/
QByteArray MainView::getClearPackage(const QByteArray& qByteArray)
{
    int openIndex, closeIndex;
    QByteArray outPutArray;
    QByteArray openByte="\n", closeByte="\v";
    if(qByteArray.contains(openByte))
    {
        openIndex = qByteArray.indexOf(openByte);
        closeIndex = qByteArray.indexOf(closeByte, openIndex);
        if((closeIndex-openIndex)==1)
        {
            outPutArray[0]=0;
            return outPutArray;
        }
        else
        {
            for(int i=openIndex+1; i<closeIndex; ++i)
            {
                outPutArray+=qByteArray[i];
            }
        }
    }
    return outPutArray;
}

/*gets a ByteArray to converting into a BitArray*/
void MainView::returnBitArray(const QByteArray& byteArray)//maybe it should be void?
{
    if(byteArray.size()>1)
    {
        int size = byteArray.size();
        QBitArray bitArray(size*8),tmp(8);//bitArray(byteArray.size()*8)
        bool times = false;
        for(int i=byteArray.size()-1; i>=0; --i)//loop repeats size of byteArray times with substructing index
        {
                times=true;
                for(int j=0; j<8; ++j)//loop for fill bitArray
                    {
                        bitArray.setBit(i*8+j, byteArray.at(i)&(1<<j));
                    }
                if(times && i==(size-1))// if it's first loop, then just pass bitArray to RESULT_ARRAY bits
                {
                    for (int c=0; c<8; c++)
                        {
                            int a = bitArray.size()-8+c;
                            if(bitArray.at(a))
                            {
                                tmp[c]=true;
                            }
                        }
                        RESULT_ARRAY|= tmp;//RESULT_ARRAY is full of false, so coping tmp int it
                        tmp.fill(false);
                }
                else
                {

                    for (int c=0; c<8; c++)
                        {
                            int a=bitArray.size()-8+c;
                            if(bitArray.at(a))
                            {
                                tmp[c]=true;
                            }
                        }
                    summingBitArrays(tmp);//summingBitArrays(bitArray);
                    tmp.fill(false);
                }
                bitArray.truncate(bitArray.size()-8);
        }
    }
}

/*convertion RESULT_ARRAY back to ByteArray*/
QByteArray MainView::returnByteArray()
{
    QByteArray outgoingArray;
    for(int b=0; b<(RESULT_ARRAY.size()); b++)
    {
        outgoingArray[b/8] = (outgoingArray[b/8] | ((RESULT_ARRAY[b]?1:0)<<(b%8)));
    }
    QByteArray reverseOutgoingArray;
    for(int k=0; k<outgoingArray.size(); k++)
    {
        reverseOutgoingArray[k]=outgoingArray[outgoingArray.size()-1-k];
    }
    RESULT_ARRAY.fill(false);
    RESULT_ARRAY.resize(8);
    return reverseOutgoingArray;
}

/*summing exisiting RESULT with bitArray*/
void MainView::summingBitArrays(const QBitArray& bitArray)
{
    QBitArray resultBitArray(RESULT_ARRAY.size());
    resultBitArray.fill(false);
    for(int i=0; i<RESULT_ARRAY.size(); i++)
    {
        if(i<7)
        {
            if(RESULT_ARRAY[i]&bitArray[i])
            {
                 resultBitArray[i+1]=true;
            }
            else if (RESULT_ARRAY[i] | bitArray[i])
            {
                if(resultBitArray[i]==0)
                {
                    resultBitArray[i]=true;
                }
                else
                {
                    resultBitArray[i]=false;
                    resultBitArray[i+1]=true;
                }
            }
        }
        else if(i==7)
        {
            if(resultBitArray[i]==1)
            {
                if(RESULT_ARRAY[i] | bitArray[i])
                {
                    if(resultBitArray.size()==8)
                    {
                        resultBitArray.resize(i+5);
                        resultBitArray[i]=false;
                        resultBitArray[i+1]=true;
                    }
                    else
                    {
                        resultBitArray[i]=false;
                        resultBitArray[i+1]=true;
                    }
                 }
            }
            else
            {
                if (RESULT_ARRAY[i]&bitArray[i])
                {
                    if(resultBitArray.size()==8)
                    {
                       resultBitArray.resize(i+5);
                    }
                    resultBitArray[i]=false;
                    resultBitArray[i+1]=true;
                }
                else if(RESULT_ARRAY[i] | bitArray[i])
                {
                    resultBitArray[i]=true;
                }
            }
        }
        else//i>7
        {
            if(i==resultBitArray.size()-1)
            {
                if(resultBitArray[i] & RESULT_ARRAY[i])
                {
                    resultBitArray.resize(i+5);
                    resultBitArray[i]=false;
                    resultBitArray[i+1]=true;
                }
            }
            else
            {
                if(resultBitArray[i] & RESULT_ARRAY[i])
                {
                    resultBitArray[i]=false;
                    resultBitArray[i+1]=true;
                }
                else if(resultBitArray[i] | RESULT_ARRAY[i])
                {
                    resultBitArray[i]=true;
                }
            }
        }
    }
    RESULT_ARRAY.fill(false);
    RESULT_ARRAY|=resultBitArray;
    resultBitArray.fill(false);
    resultBitArray.resize(8);
}
