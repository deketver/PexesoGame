#include "setter.h"
#include <QHBoxLayout>
#include<QVBoxLayout>
#include<QPushButton>
#include<QLabel>
#include<QLineEdit>
#include<QPixmap>
#include<QIcon>
#include<QDir>
#include<QSignalMapper>
#include <QDebug>
#include<QRandomGenerator>
#include <iostream>
#include<random>
#include <vector>
#include<QMessageBox>
#include<QThread>
#include<QTime>
#include<QCoreApplication>
#include"pexesochoser.h"
#include"signalsender.h"
#include <experimental/random>


//udelat v uvodu About button a info/instrukce ke vsemu

//zkontrolovat si vsude mazani a ruseni pameti, dodelat mozna destruktory, zavrit vsechny connections

setter::setter(QWidget *parent)
    : QWidget(parent),pairNumber(8), doublePlayer(true), firstChosen(-10), secondChosen(-10), firstPosition(-10),secondPosition(-10),playing(false),numberOfCol(4),numberOfCards(16),stillWinning(false),delayN(1)
{
    if(dialogSettings())
    {
        setupForm();
    }
    else
    {
        QHBoxLayout *lMain = new QHBoxLayout;
        bClose = new QPushButton(tr("&Close"));
        lMain->addWidget(bClose);
        connect(bClose, SIGNAL(clicked()), this, SLOT(close()));
        this->resize(500,500);
        this->setLayout(lMain);

        //close();
    }

}

setter::~setter()
{
    if(cardsSet.size()!= 0)
    {
         for(int i = 0; i < numberOfCards;i++)
        {
             delete buttons[i];
        }
        for(int i = 0; i < pairNumber; i++)
        {
            delete loadImages[i];
        }
    }
}

void setter::setupForm() //inicializace formulare, jednotlivych oken a tlacitek
{

    bStart = new QPushButton(tr("&Start"));
    bReset = new QPushButton(tr("&Reset"));
    bClose = new QPushButton(tr("&Close"));
    labPlayer = new QLabel(tr("Player: "));
    lWhoPlays = new QLineEdit;
    lWhoPlays->setReadOnly(true);

    connect(bClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(bStart, SIGNAL(clicked()), this, SLOT(start()));
    connect(bReset, SIGNAL(clicked()), this, SLOT(again()));

    QVBoxLayout *lMain = new QVBoxLayout;
    QHBoxLayout *lTop = new QHBoxLayout;
    lTop->addWidget(bStart);
    lTop->addWidget(bReset);
    lTop->addWidget(bClose);
    lTop->addWidget(labPlayer);
    lTop->addWidget(lWhoPlays);

    QHBoxLayout *lCenter = new QHBoxLayout;
    QVBoxLayout *lFirstPlayer = new QVBoxLayout;
    QVBoxLayout *lPlayground = new QVBoxLayout;
    QVBoxLayout *lSecondPlayer = new QVBoxLayout;
    lCenter->addLayout(lFirstPlayer);
    lCenter->addLayout(lPlayground);
    lCenter->addLayout(lSecondPlayer);

    labFirstPlayer = new QLabel(tr("Player 1 Score:"));
    lFirstScore = new QLineEdit;
    lFirstScore->setReadOnly(true);
    lFirstPlayer->addWidget(labFirstPlayer);
    lFirstPlayer->addWidget(lFirstScore);

    labSecondPlayer = new QLabel(tr("Player 2 Score:"));
    lSecondScore = new QLineEdit;
    lSecondScore->setReadOnly(true);
    lSecondPlayer->addWidget(labSecondPlayer);
    lSecondPlayer->addWidget(lSecondScore);

    project_path = QDir::currentPath();
    if(cardsSet.size()<1)
    {
        cardsSetSetter();
        late = true;
    }

    imageLoad();

    QString backgroundPath = project_path + "/img/background.png";
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(buttonPlayed(QString)));          //propojeni tlacitek pres QSignalMapper, posilani signalu a predani QStringu

    int numberOfLayouts;
    int cardCounter = 0;

    if(numberOfCards % numberOfCol == 0)
    {
        numberOfLayouts = numberOfCards/numberOfCol;
    }
    else
    {
        numberOfLayouts = (numberOfCards/numberOfCol)+1;
    }

    QHBoxLayout *fieldOfLayouts[numberOfLayouts];

    for(int i=0; i < numberOfLayouts;i++)
    {
        fieldOfLayouts[i] = new QHBoxLayout;
        int column = 0;
        while(column < numberOfCol && cardCounter < numberOfCards)
        {
            buttons[cardCounter] = new QPushButton;                   //prvni rada tlacitek
            QPixmap backgroundPix(backgroundPath);
            QIcon iconBackground(backgroundPix);
            buttons[cardCounter]->setIcon(iconBackground);
            buttons[cardCounter]->setIconSize(backgroundPix.rect().size());

            connect(buttons[cardCounter], SIGNAL(clicked(bool)), signalMapper, SLOT(map()));          //propojeni tlacitka se signalMapper
            signalMapper->setMapping(buttons[cardCounter], QString::number(cardCounter));

            fieldOfLayouts[i]->addWidget(buttons[cardCounter]);
            column++;
            cardCounter++;

        }

        lPlayground->addLayout(fieldOfLayouts[i]);
    }
    lMain->addLayout(lTop);
    lMain->addLayout(lCenter);
    this->setWindowTitle(tr("Pexeso Game!"));
    bReset->setDisabled(true);
    this->setLayout(lMain);

}

void setter::close()
{
    emit readyToQuit();
}

void setter::buttonPlayed(QString number)
{
    if(playing == true && network == false)
    {
        this->clickedCards++;

        int elementChosen = number.toInt();
        int hiddenNumber = cardsSet[elementChosen];

        if(firstChosen >= 0)
        {
            secondChosen = hiddenNumber;
        }
        else{firstChosen = hiddenNumber;}


        if(firstPosition >= 0)
        {
            secondPosition = elementChosen;
        }
        else{firstPosition = elementChosen;}

        setPicture(elementChosen, hiddenNumber);

        if(clickedCards >= 2)
        {

            if(firstChosen == secondChosen && firstPosition != secondPosition)
            {
                QMessageBox::warning(this, tr("Success!"), tr("You have found the pair!"));
                disableButton(firstPosition);
                disableButton(secondPosition);
                getPointsPlayer();
                cardsPlayed.at(firstPosition) = firstChosen;
                cardsPlayed.at(secondPosition) = firstChosen; //jsou stejne
                deleteValueInVector(firstPosition);         //smaze z vektoru moznosti pro autohrace
                deleteValueInVector(secondPosition);
                stillWinning = true;
                //switchPlayer();     //dle realne hry se hrac pak nestrida, ale hraje dal
            }
            else if(secondPosition == firstPosition)
            {
                QMessageBox::warning(this, tr("Error"), tr("You clicked the same button twice."));
                turnBackground(firstPosition);
                switchPlayer();
                stillWinning = false;
            }
            else
            {
                //QMessageBox::warning(this, tr("You missed"), tr("You missed"));

                delay();
                turnBackground(firstPosition);
                turnBackground(secondPosition);
                switchPlayer();
                stillWinning = false;

            }
            if(!doublePlayer && stillWinning != true)
            {
                autoPlayer();
            }
            firstChosen = -10;
            secondChosen = -10;
            firstPosition = -10;
            secondPosition = -10;
            clickedCards = 0;

        }

    }
    else if(playing == true && network == true)
    {
        networkGame(number);
    }
    else
    {
        QMessageBox::warning(this, tr("Game not started."), tr("Press start in order to play game."));
    }
    checkPoint();

}


void setter::randomizeCards2()              //nahodne radi vektor cisel, michacka pexesa
{

    std::mt19937 g(time(nullptr));
    std::shuffle(cardsSet.begin(), cardsSet.end(), g);

}

void setter::imageLoad()                //nacteni obrazku
{
    for(int i=0;i < pairNumber;i++)
    {
        loadImages[i] = new QPixmap(project_path+ "/img2/"+QString::number(i+1)+".png");
    }
}

void setter::start()
{
    Player = "Player 1";
    lWhoPlays->setText(Player);
    clickedCards = 0;
    firstChosen = -10;
    secondChosen = -10;
    firstPosition = -10;
    secondPosition = -10;
    firstScore = 0;
    secondScore = 0;
    lFirstScore->setText("0");
    lSecondScore->setText("0");
    playing = true;
    allSetBackground();
    if(network == false)
    {
        bReset->setDisabled(false);
        randomizeCards2();
    }
    imageLoad();
    if(network == false)
    {
        randomizeCards2();
    }
    imageLoad();
    if(youAre == "Player 1")
    {
        QMessageBox::information(this,tr("Who starts"), tr("You are starting the game."));
    }
    else if(youAre == "Player 2")
    {
        QMessageBox::information(this,tr("Who starts"), tr("Wait for the other player to chose card first."));
    }
}

void setter::setPicture(int elementChosen, int hiddenNumber)        //odkryti realneho obrazku
{

    QPixmap forLoading = *loadImages[hiddenNumber-1];
    QIcon showingReal(forLoading);
    buttons[elementChosen]->setIcon(showingReal);

    return;
}

void setter::turnBackground(int position)                   //otoceni zady
{
    QString picPath = project_path +"/img/background.png";
    QPixmap backgrundPix(picPath);
    QIcon tourningBack(backgrundPix);
    buttons[position]->setIcon(tourningBack);
    return;
}

void setter::getPointsPlayer()              //prirazeni bodu hraci
{
    QString player = lWhoPlays->text();
    if(player == "Player 1")
    {
        firstScore++;
        lFirstScore->setText(QString::number(firstScore));
        return;
    }
    else
    {
        secondScore++;
        lSecondScore->setText(QString::number(secondScore));
        return;
    }
}

void setter::switchPlayer()
{
    QString player = lWhoPlays->text();
    if(player == "Player 1")
    {
        lWhoPlays->setText("Player 2");
        Player = "Player 2";

    }
    else
    {
        lWhoPlays->setText("Player 1");
        Player = "Player 1";
    }
}

void setter::reset()
{
    clickedCards = 0;
    firstChosen = -10;
    secondChosen = -10;
    firstPosition = -10;
    secondPosition = -10;
    firstScore = 0;
    secondScore = 0;
    lFirstScore->setText("0");
    lSecondScore->setText("0");
    playing = true;
    vectorSetter();
    allSetBackground();
    randomizeCards2();
    imageLoad();

}

void setter::evaluateWinner()
{
    if(firstScore > secondScore)
    {
        QMessageBox::warning(this, tr("Player 1 winns!"), tr("The Game is over! The winner is Player 1"));
    }
    else if(firstScore == secondScore)
    {
        QMessageBox::warning(this, tr("Draw!"), tr("The Game is over! Both players have the same score!"));
    }
    else
    {
        QMessageBox::warning(this, tr("Player 2 winns!"), tr("The Game is over! The winner is Player 2"));
    }
}

void setter::again()
{
    clickedCards = 0;
    firstChosen = -10;
    secondChosen = -10;
    firstPosition = -10;
    secondPosition = -10;
    firstScore = 0;
    secondScore = 0;

    lFirstScore->setText("0");
    lSecondScore->setText("0");
    allSetBackground();
    randomizeCards2();
    imageLoad();
    playing = true;
    vectorSetter();

}

void setter::allSetBackground()
{
    QString picPath = project_path +"/img/background.png";
    QPixmap backgrundPix(picPath);
    QIcon tourningBack(backgrundPix);
    for(int i = 0; i < numberOfCards; i++)
    {
        buttons[i]->setIcon(tourningBack);
        buttons[i]->setEnabled(true);
    }
}

void setter::disableButton(int position)
{        
    buttons[position]->setEnabled(false);
}

bool setter::dialogSettings()
{
    pexesoChoser dialogWindow;
    if(dialogWindow.exec() == QDialog::Accepted)
    {
        pairNumber = dialogWindow.returnPairNumber();
        doublePlayer = dialogWindow.returnDoublePlayer();
        network = dialogWindow.returnNetwork();
        delayN = dialogWindow.returnDelay();
        if(network==true)
        {
            establish = new signalSender;
            distHost = dialogWindow.returnDistHost();
            distPort = dialogWindow.returnDistPort();
            localPort = dialogWindow.returnLocalPort();
            establish->start(localPort,distPort, distHost);

            connect(establish, SIGNAL(dataChanged()), this, SLOT(readData()));
            connect(establish, SIGNAL(sendPair()), this, SLOT(comparePairNumber()));
            connect(establish, SIGNAL(sendingVector()), this, SLOT(readVector())); //pada to na tomto, zjistit proc
            establish->sendData("Pair,"+QByteArray::number(pairNumber));

            rowSetter();        //stihnou se presunout data? neposunout jinam?
            if(localPort < distPort)
            {
                youAre = "Player 1";
                //QMessageBox::information(this,tr("Player Type"), tr("You are Player 1!"));
                vectorSetter();
                cardsSetSetter();
                randomizeCards2();
                for(int i = 0; i < (int)cardsSet.size(); i++)
                {
                    QByteArray datagram = QByteArray::number(cardsSet[i]);
                    establish->sendData("Vector,"+datagram);
                }

            }
            else
            {
                youAre = "Player 2";
                vectorSetter();
                //QMessageBox::information(this,tr("Player Type"), tr("You are Player 2!"));
            }
        }
        else if(network==false)
        {
            rowSetter();
            cardsSetSetter();
            vectorSetter();
        }
        return true;
    }
    else
    {
        //close();
       // emit readyToQuit();
        return false;

    }
}


void setter::rowSetter()
{
    numberOfCards = 2*pairNumber;
    if(numberOfCards < 22)
    {
        if(numberOfCards % 5 == 0)
        {
            numberOfCol = 5;
        }
        else
        {
            numberOfCol = 4; //jinak delim 4 a pocitam si zbytek
        }
    }
    else
    {
        numberOfCol = 6;
    }
}

void setter::vectorSetter()
{
    for(int i = 0; i < numberOfCards; i++)
    {
        cardsToPlay.push_back(i);
    }
    cardsPlayed.assign(numberOfCards, -1);
    cardsRemembered.assign(numberOfCards, -1); //na jake pozici si co zapamatoval
    return;
}


void setter::autoPlayer()
{
    int vectorSize = cardsToPlay.size();
    int random_position = std::experimental::randint(0, vectorSize);    //vybere nahodne index v moznych kartach
    firstPosition = cardsToPlay[random_position];    //jaka karta na nahodnem indexu byla vybrana
    firstChosen = cardsSet[firstPosition];    //co karta skryva za realne cislo

    setPicture(firstPosition, firstChosen); //zobrazi vybranou kartu

    QMessageBox::information(this, tr("Autoplayer chose"), tr("Autoplayer chose item."));
    int secondGuess = isInVector(firstChosen, &cardsRemembered, firstPosition);

    cardsRemembered[firstPosition] = cardsSet[firstPosition]; //toto prvni si zapamatuje

    if(secondGuess == -1) //pokud neni nalezena hodnota jiz v zapamatovanem seznamu hranych karet, tak automat vybere rovnou ji
    {
        secondGuess = std::experimental::randint(0, vectorSize);  //vybere nahodne druhou pozici

        //muze se stat, ze znovu padne vyber na to stejne policko
        while(secondGuess == random_position)
        {
            secondGuess = std::experimental::randint(0, vectorSize);
        }
        secondPosition = cardsToPlay[secondGuess];
        secondChosen = cardsSet[secondPosition];
        if(firstChosen==secondChosen)
        {
            setPicture(secondPosition, firstChosen);

            QMessageBox::warning(this, tr("Autoplayer Scored"), tr("Autoplayer received score."));
            disableButton(firstPosition);
            disableButton(secondPosition);
            getPointsPlayer();
            switchPlayer();
            cardsPlayed.at(firstPosition) = firstChosen;
            cardsPlayed.at(secondPosition) = firstChosen; //jsou stejne

            deleteValueInVector(firstPosition);
            deleteValueInVector(secondPosition);
        }
        else
        {
            setPicture(secondPosition, secondChosen);
            QMessageBox::information(this, tr("Autoplayer chose"), tr("Autoplayer chose item."));
            turnBackground(firstPosition);
            turnBackground(secondPosition);
            switchPlayer();

        }

    }
    else
    {
        setPicture(secondGuess, firstChosen);
        QMessageBox::warning(this, tr("Autoplayer Scored"), tr("Autoplayer received score."));

        secondPosition = secondGuess;
        secondChosen = firstChosen;
        disableButton(firstPosition);
        disableButton(secondPosition);
        getPointsPlayer();
        cardsPlayed.at(firstPosition) = firstChosen;
        cardsPlayed.at(secondPosition) = firstChosen; //jsou stejne
        deleteValueInVector(firstPosition);
        deleteValueInVector(secondPosition);
        switchPlayer();
    }

}

int setter::isInVector(int value, std::vector<int> *vectorSearching, int currentIndex)
{
    int vectorSize = vectorSearching->size();
    for(int i=0; i < vectorSize; i++)
    {
        if(vectorSearching->at(i) == value && i != currentIndex)
        {
            return i;
        }
    }
    return -1; //hodnota nenalezena
}

void setter::deleteValueInVector(int value)
{
    int index = isInVector(value, &cardsToPlay, 100);
    if(index != -1)
    {
        cardsToPlay.erase(cardsToPlay.begin()+index);
    }
    return;
}


void setter::readData()
{
    Message = establish->returnReceivedData();
    int indexChose = Message.toInt();
    int corrector = 0;
    if(late == true)
    {
        corrector = 2*pairNumber;
    }
    int realValue = cardsSet[indexChose+corrector];
    setPicture(indexChose, realValue);
    QMessageBox::information(this,tr("Opponent"),tr("Opponent chose a card."));
    NofOpponentChoses++;
    if(NofOpponentChoses >= 2)
    {

        NofOpponentChoses = 0;
        if(cardsSet[indexChose+corrector] == cardsSet[opponentChose+corrector] && indexChose != opponentChose)
        {
            QMessageBox::information(this,tr("Opponent"),tr("Opponent received a score!"));
            disableButton(indexChose);
            disableButton(opponentChose);
            secondScore++;
            lSecondScore->setText(QString::number(secondScore));
        }
        else if(indexChose != opponentChose)
        {
            turnBackground(indexChose);
            turnBackground(opponentChose);
        }
        else
        {
            turnBackground(indexChose);
        }
    }
    else
    {
        opponentChose = indexChose;
    }
    checkPoint();
}


void setter::readVector()
{
    QString returned = establish->returnReceivedData();
    int value;
    value = returned.section(',',1,1).toInt();
    cardsSet.push_back(value);
}

void setter::comparePairNumber()
{
    QString returned = establish->returnReceivedData();
    int value;
    value = returned.section(',',1,1).toInt();
    if(value > this->pairNumber)
    {
        this->pairNumber = value;

    }

}

void setter::networkGame(QString number)
{
    this->clickedCards++;
    int corrector =0;
    int elementChosen = number.toInt();
    if(late == true)
    {
        corrector = 2*pairNumber;
    }

    int hiddenNumber = cardsSet[elementChosen+corrector];

    if(firstChosen >= 0)
    {
        secondChosen = hiddenNumber;
    }
    else{firstChosen = hiddenNumber;}


    if(firstPosition >= 0)
    {
        secondPosition = elementChosen;
    }
    else{firstPosition = elementChosen;}

    setPicture(elementChosen, hiddenNumber);
    establish->sendData(QByteArray::number(elementChosen));

    if(clickedCards >= 2)
    {

        if(firstChosen == secondChosen && firstPosition != secondPosition)
        {
            QMessageBox::warning(this, tr("Success!"), tr("You have found the pair!"));
            disableButton(firstPosition);
            disableButton(secondPosition);
            firstScore++;
            lFirstScore->setText(QString::number(firstScore));
            stillWinning = true;
            //switchPlayer();     //zde si hraci ridi hru jak se dohodnou
        }
        else if(secondPosition == firstPosition)
        {
            QMessageBox::warning(this, tr("Error"), tr("You clicked the same button twice."));
            turnBackground(firstPosition);
            switchPlayer();
            stillWinning = false;
        }
        else
        {
            QMessageBox::warning(this, tr("You missed"), tr("You missed"));
            turnBackground(firstPosition);
            turnBackground(secondPosition);
            //switchPlayer();
            stillWinning = false;

        }
        firstChosen = -10;
        secondChosen = -10;
        firstPosition = -10;
        secondPosition = -10;
        clickedCards = 0;

    }

}

void setter::cardsSetSetter()
{
    for(int i = 1; i < pairNumber+1; i++)
       {
           cardsSet.push_back(i);
           cardsSet.push_back(i);
       }

}

void setter::checkPoint()
{
    if(firstScore + secondScore == pairNumber)
    {
         evaluateWinner();
         playing = false;

    }
}

void setter::delay()
{
    QTime dieTime= QTime::currentTime().addSecs(this->delayN);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
