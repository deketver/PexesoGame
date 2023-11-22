#ifndef PEXESOCHOSER_H
#define PEXESOCHOSER_H

#include <QDialog>
#include"signalsender.h"

class QSpinBox;
class QLabel;
class QRadioButton;
class QPushButton;

class pexesoChoser : public QDialog
{
    Q_OBJECT
    void setupForm();
    QSpinBox *bNumberOfPairs;
    QLabel *lNumberOfPairs;
    QLabel *lPlayerType;
    QLabel *lDelay;
    QSpinBox *bDelay;
    QRadioButton *bSinglePlayer;
    QRadioButton *bDoublePlayer;
    QPushButton *bOk;
    QPushButton *bClose;
    QPushButton *bNetwork;
    quint16 distPort;
    quint16 localPort;
    QString localHost;
    QString distHost;
    bool network = false;
    signalSender *settingsDone;
public:
    explicit pexesoChoser(QDialog *parent = nullptr);
    int returnPairNumber();
    bool returnDoublePlayer();
    quint16 returnDistPort();
    quint16 returnLocalPort();
    QString returnDistHost();
    int returnDelay();
    bool returnNetwork();
public slots:
    void set();
    void establishCon();
    void showInfo();
};


#endif // PEXESOCHOSER_H
