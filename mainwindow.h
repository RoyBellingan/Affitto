#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <cclassseriallib.h>
#include <cclassseriallib_modem.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    cClassSerialLIB *serial_p;  //("/dev/ttymxc4")
    cClassSerialLIB_modem *serial_s;
    QTimer *timer1;
    QTimer *timer2;
    QTimer *timerdata;
    int flag_modem;
    int flag_bluetooth;
    int flag_getstatus;
    
private:
    Ui::MainWindow *ui;

private slots:
    void modem();
    void bluetooth();
    void aggiorna1();
};

#endif // MAINWINDOW_H
