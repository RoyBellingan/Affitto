#ifndef CCLASSSERIALLIB_H
#define CCLASSSERIALLIB_H

#include <QObject>
#include <QString>

class cClassSerialLIB : public QObject
{
public:
    cClassSerialLIB(QString Porta);
    bool OpenPort(bool bloccante);
    int WritePort(QString psOutput);
    int WritePort_char(unsigned char *ptrbuffer);
    //int ReadPort(QString * Response);
    int ReadPort();
    int ReadPort_s(QString * Response_s);
    void ClosePort();
    unsigned char array_uart2 [6000];
    int counter_uart2;
    int fd;
    QString Porta;
    int len;
    int flag_bt_tolto;

};

#endif // CCLASSSERIALLIB_H
