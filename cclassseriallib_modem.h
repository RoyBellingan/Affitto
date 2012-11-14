#ifndef CCLASSSERIALLIB_MODEM_H
#define CCLASSSERIALLIB_MODEM_H

#include <QObject>
#include <QString>

class cClassSerialLIB_modem : public QObject
{
public:
    cClassSerialLIB_modem();
    bool OpenPort(bool bloccante);
    int WritePort(QString psOutput);
    int ReadPort(QString * Response);
    void ClosePort();
    int WritePort_char(unsigned char *ptrbuffer);

private:
    int fd;
};

#endif // CCLASSSERIALLIB_MODEM_H
