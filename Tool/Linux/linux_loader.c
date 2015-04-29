// Author: Ruslan Gerasimov rgerasimov@nwavetec.com   shanti_shanti@mail.ru
// Copyright: Nwave www.nwavetec.com
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define    BLOCK_SIZE    512                /* Gecko and Tiny, 'G' or 'I' */
#define    DPRINT   0


unsigned short slow_crc16(unsigned short sum,
	unsigned char *p,
	unsigned int len)
{
	while (len--)
	{
		int i;
		unsigned char byte = *(p++);
		for (i = 0; i < 8; ++i)
		{
			unsigned long oSum = sum;
			sum <<= 1;
			if (byte & 0x80)
			sum |= 1;
			if (oSum & 0x8000)
			sum ^= 0x1021;
			byte <<= 1;
		}
	}
	return sum;
}

void  main ( int argc, char *argv[] )
{


   struct stat file_info;

    if ( argc != 3 ) /* argc should be 2 for correct execution */
    {
        /* We print argv[0] assuming it is the program name */
        printf( "usage: %s filename\n", argv[0] );
    }
    else 
    {
        // We assume argv[1] is a filename to open
        //FILE *file = fopen( argv[1], "r" );
        int fd = open ( argv[1],  O_RDONLY); 
        int x;
            /* read one character at a time from file, stopping at EOF, which
               indicates the end of the file.  Note that the idiom of "assign
               to a variable, check the value" used below works because
               the assignment statement evaluates to the value assigned. */

	 /* Get information about the file.  */
  	 fstat (fd, &file_info);
         int length = file_info.st_size;
         printf("Nwave bootloader utility.\n");
         printf("File length: %d bytes.\n", length);

         int i = 0;

         unsigned char array[BLOCK_SIZE];

	 char* buffer;

	 /* Allocate a buffer large enough to hold the file's contents.  */
	 buffer = (char*) malloc (length);

         read (fd, buffer, length);
         int block_cnt = length / BLOCK_SIZE;
         int tail = length % BLOCK_SIZE;
#if DPRINT > 0
         printf("block_cnt: %d\n", block_cnt);
         printf("tail: %d\n", tail);
#endif

	//open
        int fd_serial = open( /*"/dev/ttyUSB0"*/argv[2], O_RDWR| O_NOCTTY );

	//setup

	struct termios tty;
	struct termios tty_old;
	memset (&tty, 0, sizeof tty);

	/* Error Handling */
	if ( tcgetattr ( fd_serial, &tty ) != 0 ) {
	   //std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
	}

	/* Save old tty parameters */
	tty_old = tty;

	/* Set Baud Rate */
	cfsetospeed (&tty, (speed_t)B9600);
	cfsetispeed (&tty, (speed_t)B9600);

	/* Setting other Port Stuff */
	tty.c_cflag     &=  ~PARENB;            // Make 8n1
	tty.c_cflag     &=  ~CSTOPB;
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;

	tty.c_cflag     &=  ~CRTSCTS;           // no flow control
	tty.c_cc[VMIN]   =  1;                  // read doesn't block
	tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

	/* Make raw */
	cfmakeraw(&tty);

	/* Flush Port, then applies attributes */
	tcflush( fd_serial, TCIFLUSH );
	if ( tcsetattr ( fd_serial, TCSANOW, &tty ) != 0) {
	   //std::cout << "Error " << errno << " from tcsetattr" << std::endl;
        }

  	    unsigned char ack = 0;


            write(fd_serial, "U" , 1);
#if DPRINT > 0
            printf("Wait for S-ack\n");
#endif
 	    while (ack != 0x53) {
               read(fd_serial, &ack, 1);
             // if it's a timeout without 
	    }
#if DPRINT > 0
            printf("S-ack received\n");
#endif
            write(fd_serial, "u" , 1);


            // send the len in bytes in format 32 bits = 4 bytes, e.g. 0xABCDEF10
            int *ptr = &length;
            int cnt = 0;
            write(fd_serial, ptr , 4);


            // TO DO:// send the len
            for (i = 0; i <= block_cnt - 1; i++)
               {
		   // 1. Send current block to the serial port
                    int j;
#if DPRINT > 0
                  printf("block_number: %i\n",i);
#endif
	          write(fd_serial, (buffer + i * BLOCK_SIZE) , BLOCK_SIZE);   
		  unsigned short blck_calc_crc;
                  blck_calc_crc = 0;
		  blck_calc_crc = slow_crc16(0, (buffer + i * BLOCK_SIZE) , BLOCK_SIZE);
#if DPRINT > 0
		  printf ("blck_calc_crc is: 0x%x\n",blck_calc_crc);
		  printf("wait for ack C...");
#endif
		  while (ack != 0x43) 	   {
	               read(fd_serial, &ack, 1);
		   }
#if DPRINT > 0
 		  printf("received C.\n");
#endif
		  write(fd_serial, &blck_calc_crc , 2);
		   
                   // 2. Wait for the return answer form the MCU via this serial port (block accepted confimation
                  ack = 0;
#if DPRINT > 0
		  printf("wait for ack B...");
#endif
		  while (ack != 0x42) 	   {
	               read(fd_serial, &ack, 1);
		  }
#if DPRINT > 0
   		  printf("received B.\n");
#else
                  fprintf(stderr, ".");
                  //printf("."); //
                  //fflush(stdout); // Will now print everything in the stdout buffe
#endif                   
	       }
            //3. Send the tail to the serial port
#if DPRINT > 0
           printf("Now tail:\n");
#endif
           unsigned short tail_crc = 0;
           tail_crc = slow_crc16(0, buffer + block_cnt * BLOCK_SIZE, tail);
#if DPRINT > 0
           printf ("tail_crc is: 0x%x\n",tail_crc);
#endif
	   ack = 0;

           write(fd_serial, (buffer + block_cnt * BLOCK_SIZE) , tail);


	   while (ack != 0x42) 	   {
               read(fd_serial, &ack, 1);
	   }


            //4. TO DO: find where the linker put the CRC (probably last two bytes), read out it from file(buffer)
	   unsigned short calc = 0;
           calc = slow_crc16(0, buffer, length);
#if DPRINT > 0
           printf ("calc of crc is: 0x%x\n",calc);
#endif



           //4(continue of 4). Send this CRC
           unsigned char *calc_ptr = (unsigned char*)&calc;


	   write(fd_serial, calc_ptr , 1);
#if DPRINT > 0
	   printf ("1st byte sent: x%x\n",*(unsigned char*)calc_ptr);
#endif
           ack = 0;
	   while (ack != 0x42) 	   {
               read(fd_serial, &ack, 1);
	   }
           calc_ptr++;
           write(fd_serial, calc_ptr , 1);
#if DPRINT > 0
           printf ("2nd byte sent: x%x\n",*(unsigned char*)calc_ptr);
#endif

           ack = 0;
           int flag = 1;
           
           
	   while (flag == 1) 	   {
               read(fd_serial, &ack, 1);
               if ((ack == 0x58) || (ack == 0x5A))  flag = 0;
	   }                 
           
	   

#if DPRINT > 0
           if (ack == 0x58)  printf("\nTotal CRC failed\n");
           else if (ack == 0x5A)  printf ("\nTotal CRC Succeed\n");
#else
           if (ack == 0x58)  printf("\nFlash upload failed: CRC error\n");
           else if (ack == 0x5A)  printf ("\nFlash upload successfully completed\n");
#endif           

	   write(fd_serial, "r" , 1); // Reset MCU  
	   unsigned int delay;
           for(delay = 0; delay < 0xFFFFFF; delay++); //wait just a little bit before closing the port\
           //otherwise it might be closed eariler than the last character are sent
           close (fd);
           close (fd_serial);


     }



}
