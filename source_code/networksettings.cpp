#include "networksettings.h"
#include<QtNetwork>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QLabel>
#include<QMessageBox>
#include<QUdpSocket>
#include<QPlainTextEdit>
#include<QLineEdit>


networkSettings::networkSettings(QDialog *parent) : QDialog(parent)
{
    setupForm();
}

void networkSettings::setupForm()
{
    bStart = new QPushButton(tr("&Start"));
    bClose = new QPushButton(tr("&Close"));
    bSend = new QPushButton(tr("&Ready"));
    QVBoxLayout *lMain = new QVBoxLayout;
    lMain->addWidget(bStart);
    lMain->addWidget(bClose);
    lMain->addWidget(bSend);
    statusLabel = new QLabel(tr("Ready to connect to Host Address and Port"));
    statusLabel->setWordWrap(true);

    textArea = new QPlainTextEdit;
    textArea->setReadOnly(true);
    writeField = new QLineEdit;

    lMain->addWidget(statusLabel);
    lMain->addWidget(textArea);
    lMain->addWidget(writeField);

    connect(bStart, SIGNAL(clicked()), this, SLOT(onStartClicked()));
    connect(bClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(bSend, SIGNAL(clicked()), this, SLOT(settingsReady()));

    udpSocket = new QUdpSocket(this);
    //localN = QHostAddress::LocalHost;
    //localPortN = 1243;
    udpSocket->bind(localN,localPortN);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readSocket())); //ve slotu bylo pred tim take readyRead()

    textArea->setPlainText("In the field bellow, enter the address and port you would like to connect to.");
    textArea->appendPlainText("Enter data in format 'address,port' and then click Start.");
    textArea->appendPlainText("My address is: " + localN.toString());
    textArea->appendPlainText("My PORT is: " + QString::number(localPortN));

    connect(writeField, SIGNAL(returnPressed()), this, SLOT(onEnterClicked()));
    this->setLayout(lMain);

}

void networkSettings::readSocket()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpSocket->readDatagram(datagram.data(),datagram.size(), &sender, &senderPort);
        textArea->appendPlainText("Message from: " + sender.toString() + "from port: "+ QString::number(senderPort));
        textArea->appendPlainText(QString(datagram));

    }
}

void networkSettings::onStartClicked()
{
    distPortN = writeField->text().section(',', 1, 1).toInt();
    distN = QHostAddress(writeField->text().section(',', 0, 0));
    statusLabel->setText("Port saved: "+ QString::number(distPortN));
    writeField->clear();
    textArea->clear();
    textArea->setPlainText("Address and port data saved. If you wish to send a message to the second player, use the field on the bottom. Otherwise click 'Ready'.");
    bStart->setDisabled(true);
}

void networkSettings::onEnterClicked()
{
    udpSocket->writeDatagram((QByteArray)writeField->text().toLatin1(),distN, distPortN);
    writeField->clear();
}

void networkSettings::settingsReady()
{
    if(localPortN < distPortN)
    {
        QMessageBox::warning(this,tr("Please wait."), tr("Please wait before pressing 'Ok' for the second player to set the playground."));
    }
    accept();
}


networkSettings::~networkSettings()
{
    udpSocket->close();
}

QString networkSettings::returnDistAddress()
{
    return distN.toString();
}

QString networkSettings::returnLocalAddress()
{
    return localN.toString();
}

quint16 networkSettings::returnDistPort()
{
    return distPortN;
}

quint16 networkSettings::returnLocalPort()
{
    return localPortN;
}

