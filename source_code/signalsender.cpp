#include "signalsender.h"
#include<QUdpSocket>
#include<QHostAddress>
#include <QDebug>

signalSender::signalSender(QObject *parent) : QObject(parent)
{
    //start()
    //setup();
}

void signalSender::setup()
{
    udpSocket = new QUdpSocket(this);
    //localN = QHostAddress::LocalHost;
    udpSocket->bind(localN,localPortN);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readSocket()));
}

void signalSender::start(quint16 localPort, quint16 distPort, QString distAdd)
{
    this->localPortN = localPort;
    this->distPortN = distPort;
    this->distN = QHostAddress(distAdd);//QHostAddress::Broadcast;
    setup();
}

void signalSender::readSocket()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;
        udpSocket->readDatagram(datagram.data(),datagram.size(), &sender, &senderPort);
        receivedData = QString(datagram);
        qDebug() << datagram;
        if(receivedData.section(',', 0, 0) == "Vector") //filtrovani prijatych dat
        {
            emit sendingVector();
        }
        else if(receivedData.section(',', 0, 0) == "Pair")
        {
            emit sendPair();
        }
        else if(datagram == "")
        {;}
        else if(datagram == "Finished")
        {
            emit readyPress();
        }
        else
        {
            emit dataChanged();
        }
    }
}

void signalSender::sendData(QByteArray datagram)
{
    udpSocket->writeDatagram(datagram,distN, distPortN);
    sentData = datagram;

}

signalSender::~signalSender()
{
    udpSocket->close();
}

QString signalSender::returnReceivedData()
{
    return receivedData;
}
