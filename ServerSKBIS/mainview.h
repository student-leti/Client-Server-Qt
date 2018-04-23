#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QMainWindow>

namespace Ui {
class MainView;
}

class QTcpServer;
class QTcpSocket;

class MainView : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = 0);
    ~MainView();

private slots:
    void on_btnStartServer_clicked();

    void on_btnStopServer_clicked();

private:
    Ui::MainView *ui;
    QTcpServer* m_server;
    QTcpSocket* m_socket;
    bool StartServer();
    void StopServer();
    void ExchangeData();
    void EchoReadData();
    void returnBitArray(const QByteArray &byteArray);
    QByteArray getClearPackage(const QByteArray &qByteArray);
    QByteArray returnByteArray();
    QBitArray bytesToBits(const QByteArray &byteArray);
    void summingBitArrays(const QBitArray &bitArray);
    QBitArray changeBitIndex(const QBitArray &bitArray);
    QByteArray getAnswer(const QByteArray &temp);
};

#endif // MAINVIEW_H
