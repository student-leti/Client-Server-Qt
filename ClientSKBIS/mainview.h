#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QMainWindow>

namespace Ui {
class MainView;
}

class QTcpSocket;


class MainView : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = 0);
    ~MainView();

    //void SetOpenNoiseParameter(quint16 value);
    //void SetCloseNoiseParameter(quint16 value);
    //void SetMessageParameter(quint16 value);

Q_SIGNALS:

signals:
    //void closed();
//    void NotifyOpenNoiseParameterChange(quint16 value);
//    void NotifyCloseNoiseParameterChange(quint16 value);
//    void NotifyDataParametersChange(quint16 value);
//    void NotifyMessageChange(const QString& value);

private slots:

    void on_btnConnect_clicked();

    void on_btnDisconnect_clicked();

    void onConnected();

    void onDisconnect();

    //void bytesWritten(qint64 bytes);

    void on_btnSend_clicked();

    //void on_editOpenNoise_textChanged(quint16 value);

    //void on_editCloseNoise_textChanged(quint16 value);

    //void on_editDataParameters_textChanged(quint16 value);

    //void on_editMessage_textChanged(const QString& value);

private:
    Ui::MainView *ui;
    QTcpSocket *m_socket;


    void SendData();

    QString CollectMessage();

    void SetConnection();

    void readyRead();

    int GetOpenNoiseParameter();

    int GetMessageParameter();

    int GetMessageLenght();

    int GetCloseNoiseParameter();

    bool CompareLegth();

    QString GetMessage();

    QString SaltFunction();

    QString PepperFunction();
};

#endif // MAINVIEW_H
