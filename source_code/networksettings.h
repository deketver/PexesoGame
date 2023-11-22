#ifndef NETWORKSETTINGS_H
#define NETWORKSETTINGS_H

#include <QDialog>
#include<QHostAddress>

class QHostAddress;
class QPushButton;
class QUdpSocket;
class QLabel;
class QPlainTextEdit;
class QLineEdit;

class networkSettings : public QDialog
{
    Q_OBJECT
    QPushButton *bStart;
    QPushButton *bClose;
    void setupForm();
    QUdpSocket *udpSocket = nullptr;
    QHostAddress localN = QHostAddress::LocalHost;
    quint16 localPortN = 1243;
    QHostAddress distN;// = QHostAddress::LocalHost;
    quint16 distPortN;
    QLabel *statusLabel;
    QPlainTextEdit *textArea;
    QLineEdit *writeField;
    QPushButton *bSend;
public:
    explicit networkSettings(QDialog *parent = nullptr);
    ~networkSettings();
    QString returnLocalAddress();
    QString returnDistAddress();
    quint16 returnLocalPort();
    quint16 returnDistPort();
private slots:
    void onEnterClicked();
    void onStartClicked();
public slots:
    void readSocket();
    void settingsReady();
signals:

};

#endif // NETWORKSETTINGS_H
