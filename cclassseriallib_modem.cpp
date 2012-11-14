#include "cclassseriallib_modem.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>


cClassSerialLIB_modem::cClassSerialLIB_modem()
{

}

bool cClassSerialLIB_modem::OpenPort(bool bloccante)
{
      // make sure port is closed
      ClosePort();

      fd = open("/dev/ttyUSB3", O_RDWR |
                          O_NOCTTY | /* tells UNIX that this program doesn't want to be the "controlling terminal" for that port*/
                          O_NDELAY); /* tells UNIX that this program doesn't care what state the DCD signal line is in */

      if (fd < 0)
      {
          printf("open error %d %s\n", errno, strerror(errno));
          return false;
      }
      else
      {
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

          tcgetattr(fd, &options);

          //set port speed
          cfsetispeed(&options, B115200);
          cfsetospeed(&options, B115200);

          options.c_cflag |= (CLOCAL | CREAD);

          //set 8n1
          options.c_cflag &= ~PARENB; /* Enable parity bit */
          options.c_cflag &= ~CSTOPB; /* 	2 stop bits (1 otherwise) */
          options.c_cflag &= ~CSIZE; /* Mask the character size bits */
          options.c_cflag |= CS8; /* Select 8 data bits */


           /*options.c_cflag &= ~CRTSCTS;
           options.c_iflag &= ~IXON; //RIMUOVE X0N/XOFF control
           options.c_iflag &= ~IXOFF;
           options.c_iflag &= ~IGNCR; //\n\r*/


          //set raw input
          options.c_lflag &= ~(ICANON | /* Enable canonical input (else raw) */
                                  ECHO | /*Enable echoing of input characters	*/
                                  ECHOE | /*Echo erase character as BS-SP-BS*/
                                  ISIG); /*Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals*/

          tcsetattr(fd, TCSANOW, &options); /*Make changes now without waiting for data to complete*/
      }
      return true;
}

int cClassSerialLIB_modem::WritePort(QString psOutput)
{
    int iOut;
    QByteArray ba = psOutput.toLocal8Bit();

    if (fd < 1)
    {
        printf(" port is not open\n");
        return -1;
    } // end if

    iOut = write(fd, ba.data(), strlen(ba.data()));
    if (iOut < 0)
    {
        printf("write error %d %s\n", errno, strerror(errno));
    }
    else
    {
        printf("wrote %d chars: %s\n", iOut, ba.data());
    } // end if
    return iOut;
}

int cClassSerialLIB_modem::ReadPort(QString * Response)
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

       *Response=QString::fromLocal8Bit(psResponse);

       return iIn;
}

void cClassSerialLIB_modem::ClosePort()
{
    // you may want to restore the saved port attributes
    if (fd > 0)
    {
        close(fd);
    } // end if
}

int cClassSerialLIB_modem::WritePort_char(unsigned char *ptrbuffer)
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
