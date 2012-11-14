#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
#include "cclassseriallib.h"
#include <QDebug>

cClassSerialLIB::cClassSerialLIB(QString Porta)
{
    this->Porta=Porta;
    flag_bt_tolto=0;
    for (int i=0;i<6000;i++)
             array_uart2[i]='\0';
}

bool cClassSerialLIB::OpenPort(bool bloccante)
{
      // make sure port is closed
      ClosePort();
      //ttymxc4
      QByteArray Porta_byte_array = Porta.toLatin1();
      const char* Porta_char=Porta_byte_array.data();
      fd = open(Porta_char/*"/dev/ttymxc4"*/, O_RDWR |
                          O_NOCTTY | /* tells UNIX that this program doesn't want to be the "controlling terminal" for that port*/
                          O_NDELAY); /* tells UNIX that this program doesn't care what state the DCD signal line is in */

      if (fd < 0)
      {
          qDebug("errore in apertura %d %s\n", errno, strerror(errno));
          return false;
      }
      else
      {
          qDebug("ok %d",fd);
          struct termios options;

          /* The FNDELAY option causes the read function to return 0 if no
            characters are available on the port. To restore normal (blocking) behavior,
            call fcntl() without the FNDELAY option: */

          if(bloccante)
          {
              printf("bloccante\n");
              fcntl(fd, F_SETFL, 0);
          }
          else
          {
             printf("NON bloccante");
             fcntl(fd, F_SETFL, FNDELAY);
          }

          //cfmakeraw(&options); //ABILITARLO NEL CASO DI ERRORE!


          tcgetattr(fd, &options);

          //set port speed 115200
          cfsetispeed(&options, B115200);
          cfsetospeed(&options, B115200);

          options.c_cflag |= (CLOCAL | CREAD);

          //set 8n1

          options.c_cflag &= ~PARENB; /* Disable parity bit */
          options.c_cflag &= ~CSTOPB; /* 1 stop bits (1 otherwise) */
          options.c_cflag &= ~CSIZE; /* Mask the character size bits */
          options.c_cflag |= CS8; /* Select 8 data bits */

          options.c_iflag &= ~IXON; //RIMUOVE X0N/XOFF control
          options.c_iflag &= ~IXOFF;
          options.c_iflag &= ~IGNCR;
          options.c_iflag &= ~INPCK;

          //set raw input
          options.c_lflag &= ~(ICANON |   /*Enable canonical input (else raw)*/
                                  ECHO |  /*Enable echoing of input characters*/
                                  ECHOE | /*Echo erase character as BS-SP-BS*/
                                  ISIG);  /*Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals*/

         tcsetattr(fd, /*TCSAFLUSH*/TCSANOW, &options); /*Make changes now without waiting for data to complete*/

      }
      return true;
}

int cClassSerialLIB::WritePort(QString psOutput)
{
    int iOut;
    QByteArray ba = psOutput.toLocal8Bit();

    if (fd < 1)
    {
        qDebug(" port is not open\n");
        return -1;
    } // end if

    //int strbadata = strlen(ba.data());
    iOut = write(fd, ba.data(), strlen(ba.data()));


    if (iOut < 0)
    {
        qDebug("errore scrittura %d %s\n", errno, strerror(errno));
    }
    else
    {
        qDebug("wrote %d chars: %s\n", iOut, ba.data());
    } // end if
    return iOut;
}

int cClassSerialLIB::ReadPort(/*QChar * Response*/)
{
     long int iIn;

     counter_uart2 =0;

     /*for (int i=0;i<6000;i++)
         array_uart2[i]='\0';
    */
     //unsigned char psResponse[130000];

       if (fd < 1)
       {
           printf(" port is not open\n");
           return -1;
       } // end if

       iIn = read(fd, array_uart2, sizeof(array_uart2));

       static int count=0;
/*
       if (iIn==-1){
           count++;
           if (count==40)
               flag_bt_tolto=1;
       }
       else
           count=0;
*/


       if (iIn < 0)
       {
           if (errno == EAGAIN)
           {         
                   //qDebug("Read nothing");
                   return 0; // assume that command generated no response
           }
           else
           {
                   printf("read error %d %s\n", errno, strerror(errno));
           } // end if
       }
#if 0
       else
       {
           psResponse[iIn<254?iIn:254] = '\0';
           //qDebug("read %d chars: %s\n", iIn, psResponse);
       } // end if

       if (counter_uart2+iIn<130000){
       //*Response=QString::fromLocal8Bit(psResponse);
       for (int i=0;i <iIn; i++)
           array_uart2[counter_uart2++]=psResponse[i];

           //array[counter]=0;
           //QString a(array);
           //qDebug()<<a;

       }
       else
          counter_uart2=0;
#endif
       counter_uart2=iIn;
       return iIn;
}

void cClassSerialLIB::ClosePort()
{
    // you may want to restore the saved port attributes
    if (fd > 0)
    {
        close(fd);
    } // end if
}

int cClassSerialLIB::WritePort_char(unsigned char *ptrbuffer)
{
    int iOut;

    if (fd < 1)
    {
        qDebug(" port is not open\n");
        return -1;
    } // end if

    iOut = write(fd, ptrbuffer, 1);
    if (iOut < 0)
    {
        qDebug("write error %d %s\n", errno, strerror(errno));
    }
    else
    {
        qDebug("wrote %d chars: %d\n", iOut, *ptrbuffer);
    } // end if
    return iOut;
}

int cClassSerialLIB::ReadPort_s(QString * Response_s)
{

     int iIn;
     char psResponse[254];

       if (fd < 1)
       {
           printf(" port is not open\n");
           return -1;
       } // end if

       iIn = read(fd, psResponse, 254);

       if (iIn < 0)
       {
           if (errno == EAGAIN)
           {
                   return 0; // assume that command generated no response
           }
           else
           {
                   printf("read error %d %s\n", errno, strerror(errno));
           } // end if
       }
       else
       {
           psResponse[iIn<254?iIn:254] = '\0';
           printf("read %d chars: %s\n", iIn, psResponse);
       } // end if

       *Response_s=QString::fromLocal8Bit(psResponse);

       return iIn;
}


