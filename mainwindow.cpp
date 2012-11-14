#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDebug>
#include <stdlib.h>
#include <QTime>

#define porta "9940"
#define ip "82.91.28.127"
//#define modemsocket_bt 1
#define bt_solo 1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*DA METTERE NELL RC_LOCAL*/
    system("echo 165 > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio165/direction");
    system("echo 0 > /sys/class/gpio/gpio165/value");

    /*SERIALE_BLUETOOTH*/
    serial_p= new cClassSerialLIB(QString("/dev/ttymxc4"));

    /*SERIALE MODEM*/
    serial_s= new cClassSerialLIB_modem();

    timer1=new QTimer(this);
    timer2=new QTimer(this);

    flag_modem=0;
    flag_bluetooth=0;

#ifdef modemsocket_bt
    connect(timer1, SIGNAL(timeout()), this, SLOT(modem()));
    timer1->start(1000);
#endif

#ifdef bt_solo
    //connect(timer2, SIGNAL(timeout()), this, SLOT(bluetooth()));
    //timer2->start(1000);
    this->bluetooth();

#endif
}

void MainWindow::modem()
{
 if (flag_modem==0){
    if(serial_s->OpenPort(1)==false)
        qDebug("Errore apertura seriale Modem");
    else{
        QString data;
        data="at+cgreg?\n";

        serial_s->WritePort(data);
        sleep(1);

        QString data2;

        serial_s->ReadPort(&data2);

        qDebug()<<data2;

        QString padsetup=QString("at!padsetup=1,2,%1,0,%2,0,1\n").arg(ip).arg(porta);
        serial_s->WritePort(padsetup);
        sleep(1);

        //serial_s->ReadPort(&data2);
        //qDebug()<<data2;

        serial_s->WritePort("at+cgdcont=1,\"IP\",\"web.omnitel.it\"\n");
        sleep(1);
        //serial_s->ReadPort(&data2);
        //qDebug()<<data2;

        serial_s->WritePort("at!padconn=1\n");
        sleep(10);
        /*controllo se la risposta è connect pad: 1,0 e setta flag_modem=1*/
        serial_s->ReadPort(&data2);

        if(data2.contains("CONNECT")){//[15]=='O' && data2[16]=='K')
            flag_modem=1;
            qDebug("MODEM CONNESSO AL SOCKET");
            //timer2->singleShot(50,this,SLOT(bluetooth()));
            this->bluetooth();
        }
        else{
            QString error = QString("Impossibile aprire la Porta %1 sul Server %2").arg(porta).arg(ip);
            qDebug()<<error;
        }
    }
  }


}


void MainWindow::bluetooth(){

    if (serial_p->OpenPort(0)==false)//0 non bloccante
        qDebug("Errore Apertura Seriale WT111");
    else{
        QString response;
        /*
        serial_p->WritePort("INQUIRY 5\n");
        sleep(11);
        serial_p->ReadPort_s(&response);
        qDebug()<<response;
        sleep(1);

        serial_p->WritePort("SET BT PAIR\n");
        sleep(1);
        serial_p->WritePort("KILL 00:03:19:9e:8b:71\n");
        sleep(1);
        serial_p->ReadPort_s(&response);
        qDebug()<<response;*/
        serial_p->WritePort("SET PROFILE SPP ON\n");
        sleep(1);
        //serial_p->WritePort("PAIR 00:03:19:9e:8b:71\n");
        //sleep(1);
        serial_p->WritePort("RESET\n");
        sleep(1);
        serial_p->WritePort("SET BT AUTH * mamma\n");//mamma\n
        sleep(1);
        serial_p->WritePort("CALL 00:03:19:9e:8b:71 1101 RFCOMM\n");
        sleep(5);
        serial_p->ReadPort_s(&response);
        qDebug()<<response;
        /*if (response.contains("CONNECT 0 RFCOMM 1"))
                flag_bluetooth=1;

        qDebug()<<QString::number(flag_bluetooth,10);*/

/**** GET STATUS *****/

#if 0

        unsigned char ptrbuffer[8];
        ptrbuffer[0]=0x4D;//M
        ptrbuffer[1]=0x41;//A
        ptrbuffer[2]=0x4D;//M
        ptrbuffer[3]=0x01;
        ptrbuffer[4]=0x00;
        ptrbuffer[5]=0x03;
        ptrbuffer[6]=0x6D;
        ptrbuffer[7]=0x4B;


        serial_p->WritePort_char (&ptrbuffer[0]);
        serial_p->WritePort_char (&ptrbuffer[1]);
        serial_p->WritePort_char (&ptrbuffer[2]);
        serial_p->WritePort_char (&ptrbuffer[3]);
        serial_p->WritePort_char (&ptrbuffer[4]);
        serial_p->WritePort_char (&ptrbuffer[5]);
        serial_p->WritePort_char (&ptrbuffer[6]);
        serial_p->WritePort_char (&ptrbuffer[7]);

        sleep(2);

        serial_p->ReadPort();
        /*int b0lsb = serial_p->array_uart2[7];
        int b0msb = serial_p->array_uart2[8];
        unsigned short shift_dx_b0lsb = b0lsb>>8;
        unsigned short shift_sx_b0msb = b0msb<<8;
        unsigned short controller_battery=shift_sx_b0msb|shift_dx_b0lsb;


        QString a = QString("Livello Batteria: %1 mV\n\n").arg(controller_battery);
        */
        QString a = QString("Livello Batteria: %1, %2").arg((int)serial_p->array_uart2[7]).arg((int)serial_p->array_uart2[8]);
        qDebug()<<a;

        //serial_p->counter_uart2 = 0;//resetta counter

#endif

/******START ECG*******/
        unsigned char ptrbuffer[8];

        ptrbuffer[0]=0x4D;//M
        ptrbuffer[1]=0x41;//A
        ptrbuffer[2]=0x4D;//M
        ptrbuffer[3]=0x01;
        ptrbuffer[4]=0x00;
        ptrbuffer[5]=0x01;
        ptrbuffer[6]=0x6D;
        ptrbuffer[7]=0x49;

etichetta:

        serial_p->WritePort_char (&ptrbuffer[0]);
        serial_p->WritePort_char (&ptrbuffer[1]);
        serial_p->WritePort_char (&ptrbuffer[2]);
        serial_p->WritePort_char (&ptrbuffer[3]);
        serial_p->WritePort_char (&ptrbuffer[4]);
        serial_p->WritePort_char (&ptrbuffer[5]);
        serial_p->WritePort_char (&ptrbuffer[6]);
        serial_p->WritePort_char (&ptrbuffer[7]);
        sleep(1);
        serial_p->ReadPort();
        qDebug("\t%d\n",serial_p->array_uart2[5]);

       if (serial_p->array_uart2[5]==129){

            for (int i=0;i<6000;i++)
                     serial_p->array_uart2[i]='\0';

            flag_getstatus=1;

            timerdata = new QTimer(this);
            connect(timerdata, SIGNAL(timeout()), this, SLOT(aggiorna1()));
            timerdata->start(80);

       }
        else{
           //this->bluetooth();
#if 1
            unsigned char reset[2];
            reset[0]=0x0D;
            reset[1]=0x0A;
            serial_p->WritePort("AT+JRES");
            serial_p->WritePort_char(&reset[0]);
            serial_p->WritePort_char(&reset[1]);
            sleep(4);
#endif
           goto etichetta;
        }
  }//chiusura else iniziale

}//chiusura funzione


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::aggiorna1(){
    QTime t;
    t.start();

    for (int i=0;i<6000;i++)
          serial_p->array_uart2[i]='\0';

    serial_p->ReadPort();

    //for(int j=0;j<6000;j++){
      int n_samples=0;
      int m_pos;



      unsigned char buffer[2000];
      for (int i=0;i<2000;i++)
          buffer[i]='\0';

      int counter=0;


 for (int j=0;j<6000;j++){
     n_samples=serial_p->array_uart2[j+7];//numero sample.
     int len_payload=serial_p->array_uart2[j+3];
     m_pos=len_payload+5;
     if (serial_p->array_uart2[j]=='M' && serial_p->array_uart2[j+1]=='A'&& serial_p->array_uart2[j+2]=='M'&& serial_p->array_uart2[j+5]==128 && serial_p->array_uart2[j+m_pos]=='m'/*&& Buffer_temp[j+6]==7*/){
        qDebug("Trovato\n");
        counter=0;
        for (int i=0;i<2000;i++)
             buffer[i]=serial_p->array_uart2[i];


        QString str="";
        QString temp="";
        int num_byte=n_samples*3/2; //numero_byte
        for(int i=j+13; i<j+num_byte+13; i+=3){
            int b0_msb= buffer[i];//8bit
            unsigned short shift_dx_b0msb = b0_msb<<4;
            int b0_lsb= buffer[i+1] & 0xF0;//4bit
            unsigned short shift_dx_b0lsb = b0_lsb>>4;
            unsigned short first_sample=shift_dx_b0msb|shift_dx_b0lsb;
            int b1_msb= buffer[i+1] & 0x0F;//4bit
            unsigned short shift_dx_b1msb = b1_msb<<8;
            int b1_lsb= buffer[i+2];			//8bit
            unsigned short shift_dx_b1lsb = b1_lsb;//>>4;
            unsigned short second_sample=shift_dx_b1msb|shift_dx_b1lsb;
            //str="";//gius
            str = QString("%1,%2").arg(first_sample).arg(second_sample);
            temp.append(str);
            temp.append(", ");
        }//chiusura for
#ifdef bt_solo

        qDebug()<<temp;
#endif
        /*scrittura dati sul modem*/
#ifdef modemsocket_bt
        serial_s->WritePort(temp);
#endif

        }//chiusura if


      else{
          counter++;
         }


    }
    qDebug()<<counter;
    qDebug("\nTempo Trascorso %d ms\n",t.elapsed());

 //for (int i=0;i<6000;i++)
      //serial_p->array_uart2[i]='\0';

}
