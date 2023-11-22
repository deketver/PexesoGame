#ifndef SIGNALSENDER_H
#define SIGNALSENDER_H

#include <QObject>
#include<QHostAddress>

class QUdpSocket;

class signalSender : public QObject
{
    Q_OBJECT
    QUdpSocket *udpSocket;
    QHostAddress localN =  QHostAddress::LocalHost;;
    quint16 localPortN = 1243;
    QHostAddress distN;// = QHostAddress::LocalHost;
    quint16 distPortN;// =1244;
    void setup();
    QString receivedData = "";
    QString sentData = "";
public:
    explicit signalSender(QObject *parent = nullptr);
    void start(quint16 localPort, quint16 distPort, QString distAdd);
    void sendData(QByteArray datagram);
    QString returnReceivedData();
    ~signalSender();
public slots:
    void readSocket();
signals:
    void dataChanged();
    void sendingVector();
    void sendPair();
    void readyPress();
};

#endif // SIGNALSENDER_H
