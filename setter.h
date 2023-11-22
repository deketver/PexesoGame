#ifndef SETTER_H
#define SETTER_H

#include <QWidget>

class QPushButton;
class QLabel;
class QLineEdit;
class QSignalMapper;
class signalSender;

class setter : public QWidget
{
    Q_OBJECT
    void setupForm();
    QPushButton *bStart;
    QPushButton *bReset;
    QPushButton *bClose;
    QLabel *labPlayer;
    QLabel *labFirstPlayer;
    QLabel *labSecondPlayer;
    QLineEdit *lWhoPlays;
    QLineEdit *lFirstScore;
    QLineEdit *lSecondScore;
    QString Player;
    int pairNumber;
    int clickedCards;
    bool doublePlayer;
    std::vector<int> cardsSet;
    std::vector<int> cardsPlayed; //pole, ktere uz byly zahrane
    std::vector<int> cardsToPlay; //policka, ktera bude hrat autoplayer
    std::vector<int> cardsRemembered; //policka, jejichz pozici si zapamatoval automat
    QSignalMapper *signalMapper;
    QString project_path;
    void randomizeCards2();
    QPixmap *loadImages[16];
    void imageLoad();
    QPushButton *buttons[32];
    int firstChosen;
    int secondChosen;
    int firstPosition;
    int secondPosition;
    bool playing;
    void setPicture(int what, int hidden);
    void turnBackground(int position);
    void getPointsPlayer();
    void switchPlayer();
    void cardsSetSetter();
    int firstScore;
    int secondScore;
    void reset();
    void evaluateWinner();
    void allSetBackground();
    void disableButton(int position);
    bool dialogSettings();
    void rowSetter();
    int numberOfCol;
    int numberOfCards;
    void vectorSetter();
    void autoPlayer();
    int isInVector(int value, std::vector<int> *vectorSearching, int currentIndex);
    void deleteValueInVector(int value);
    bool stillWinning;
    bool network;
    quint16 distPort;//  = 1244;
    quint16 localPort;
    QString localHost;
    QString distHost;
    signalSender *establish;
    QString Message;
    QString youAre = "";
    int NofOpponentChoses = 0;
    int opponentChose = 0;
    void networkGame(QString number);
    bool late = false;
    void checkPoint();
    int delayN;
    void delay();
signals:
    void readyToQuit();
    void clicked(int x);
public:
    setter(QWidget *parent = nullptr);
    ~setter();
public slots:
    void close();
    void buttonPlayed(QString number);
    void start();
    void again();
    void readData();
    void readVector();
    void comparePairNumber();
};
#endif // SETTER_H
