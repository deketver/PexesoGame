#include "pexesochoser.h"
#include"networksettings.h"
#include<QLabel>
#include<QSpinBox>
#include<QVBoxLayout>
#include<QMessageBox>
#include<QRadioButton>
#include<QPushButton>

pexesoChoser::pexesoChoser(QDialog *parent) : QDialog(parent)
{
    setupForm();
}

void pexesoChoser::setupForm()
{
    lNumberOfPairs = new QLabel(tr("Choose the number of pexeso cards pairs: "));
    bNumberOfPairs = new QSpinBox;
    bNumberOfPairs->setMinimum(4);
    bNumberOfPairs->setMaximum(16);
    lPlayerType = new QLabel(tr("Choose type of the game: "));
    bSinglePlayer = new QRadioButton(tr("Single Player"));
    bDoublePlayer = new QRadioButton(tr("Double Player"));
    bDoublePlayer->setChecked(true);
    bOk = new QPushButton(tr("&OK"));
    bClose = new QPushButton(tr("&Close"));
    bNetwork = new QPushButton(tr("&Connect via network"));
    lDelay = new QLabel(tr("Set tourning delay (in s): "));
    bDelay = new QSpinBox;
    bDelay->setMinimum(1);
    bDelay->setMaximum(10);


    connect(bClose, SIGNAL(clicked()), this, SLOT(reject()));
    connect(bOk,SIGNAL(clicked()), this, SLOT(set()));
    connect(bNetwork, SIGNAL(clicked()), this, SLOT(establishCon()));

    QHBoxLayout *lMain = new QHBoxLayout;
    QVBoxLayout *lLeft = new QVBoxLayout;
    QVBoxLayout *lRight = new QVBoxLayout;
    //lLeft->addStretch();
    lRight->addStretch();



    lLeft->addWidget(lPlayerType);
    lRight->addWidget(bSinglePlayer);
    lRight->addWidget(bDoublePlayer);
    lRight->addWidget(bNetwork);

    lLeft->addWidget(lNumberOfPairs);
    lRight->addWidget(bNumberOfPairs);

    lLeft->addWidget(lDelay);
    lRight->addWidget(bDelay);

    lLeft->addWidget(bOk);
    lRight->addWidget(bClose);

    lMain->addLayout(lLeft);
    lMain->addLayout(lRight);
    this->setWindowTitle(tr("Pexeso Settings"));
    this->setLayout(lMain);

}

void pexesoChoser::set()
{
    if(network == true)
    {
        settingsDone->sendData("Finished");
    }
    accept();
}

int pexesoChoser::returnPairNumber()
{
    return (bNumberOfPairs->text()).toInt();
}

bool pexesoChoser::returnDoublePlayer()
{
    if(bDoublePlayer->isChecked() == true)
    {
        return true;
    }
    else if(bSinglePlayer->isChecked()== true)
    {
        return false;
    }
    else return true;
}

void pexesoChoser::establishCon()
{
    networkSettings sender;
    if(sender.exec() == QDialog::Accepted)
    {
        distPort = sender.returnDistPort();
        localPort = sender.returnLocalPort();
        distHost = sender.returnDistAddress();
        localHost = sender.returnLocalAddress();
        bSinglePlayer->setDisabled(true);
        network = true;
        settingsDone = new signalSender;
        settingsDone->start(localPort,distPort,distHost);
        connect(settingsDone, SIGNAL(readyPress()), this, SLOT(showInfo()));
    }
    else
    {
        close();

    }
}

quint16 pexesoChoser::returnDistPort()
{
    return distPort;
}

quint16 pexesoChoser::returnLocalPort()
{
    return localPort;
}

QString pexesoChoser::returnDistHost()
{
    return distHost;
}

bool pexesoChoser::returnNetwork()
{
    return network;
}

void pexesoChoser::showInfo()
{
    QMessageBox::warning(this, tr("Ready to start!"), tr("Settings are ready, now you can pres 'Ok'"));
}

int pexesoChoser::returnDelay()
{
    return (bDelay->text()).toInt();
}
