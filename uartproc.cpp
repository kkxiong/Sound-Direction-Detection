#include <stdio.h>
#include <string.h>
#include <unistd.h>      //Used for UART
#include <fcntl.h>      //Used for UART
#include <termios.h>    //Used for UART
#include <errno.h>

#define error_message printf
int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                error_message ("error %d setting term attributes", errno);
}


char portname[] = "/dev/ttyACM2";
int uart0_filestream = -1;

bool uartopen()
{
  //-------------------------
  //----- SETUP USART 0 -----
  //-------------------------
  //At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively

  //OPEN THE UART
  //The flags (defined in fcntl.h):
  //  Access modes (use 1 of these):
  //    O_RDONLY - Open for reading only.
  //    O_RDWR - Open for reading and writing.
  //    O_WRONLY - Open for writing only.
  //
  //  O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
  //                      if there is no input immediately available (instead of blocking). Likewise, write requests can also return
  //                      immediately with a failure status if the output can't be written immediately.
  //
  //  O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.

  uart0_filestream = open("/dev/ttyACM1", O_RDWR | O_NOCTTY | O_SYNC);    //Open in non blocking read/write mode
  if (uart0_filestream == -1)
  {
      uart0_filestream = open("/dev/ttyACM2", O_RDWR | O_NOCTTY | O_SYNC);
      if (uart0_filestream == -1)
      {
                uart0_filestream = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_SYNC);
            }
  }
  if (uart0_filestream == -1)
  {
    //ERROR - CAN'T OPEN SERIAL PORT
    printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
    return false;
  }

  //CONFIGURE THE UART
  //The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
  //  Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
  //  CSIZE:- CS5, CS6, CS7, CS8
  //  CLOCAL - Ignore modem status lines
  //  CREAD - Enable receiver
  //  IGNPAR = Ignore characters with parity errors
  //  ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
  //  PARENB - Parity enable
  //  PARODD - Odd parity (else even)
  struct termios options;
  tcgetattr(uart0_filestream, &options);
  options.c_cflag = B9600 | CS8 | CLOCAL;    //<Set baud rate
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  //tcflush(uart0_filestream, TCIFLUSH);
  //tcsetattr(uart0_filestream, TCSANOW, &options);

        set_interface_attribs (uart0_filestream, B9600, 0);  // set speed to 9600 bps, 8n1 (no parity)
        set_blocking (uart0_filestream, 0);                // set no blocking

  return true;
}

void uartclose()
{
  if (uart0_filestream != -1) close(uart0_filestream);
}

bool uartout(int8_t data)
{
  if (uart0_filestream != -1)
  {
    int count = write(uart0_filestream, &data, 1);    //Filestream, bytes to write, number of bytes to write
    if (count < 0)
    {
      printf("UART TX error\n");
      uartclose();
      uart0_filestream = open("/dev/ttyACM1", O_RDWR | O_NOCTTY | O_SYNC);    //Open in non blocking read/write mode
      if (uart0_filestream == -1)
      {
        uart0_filestream = open("/dev/ttyACM2", O_RDWR | O_NOCTTY | O_SYNC);
        if (uart0_filestream == -1)
        {
          uart0_filestream = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_SYNC);
          if (uart0_filestream == -1)
          return false;
        }
      }
    }
    usleep(100000);
    //sleep(1);
    return true;
  }
  return false;
}

