// Author: Ruslan Gerasimov rgerasimov@nwavetec.com   shanti_shanti@mail.ru
// Copyright: Nwave www.nwavetec.com
// To compile with MinGW:
//
//      gcc -o winloader.exe winloader.c
//
// To compile with cl, the Microsoft compiler:
//
//      cl winloader.cpp
//
// To run:
//
//      winloader.exe
//
 
#include <windows.h>
#include <stdio.h>
#define    BLOCK_SIZE    512 // 512;                /* Gecko and Tiny, 'G' or 'I' */
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






 
//int main()
int main (int argc, char *argv[])
{
	    int i;
	    // Define the five bytes to send ("hello")
	    char bytes_to_send[5];
	    bytes_to_send[0] = 104;
	    bytes_to_send[1] = 101;
	    bytes_to_send[2] = 108;
	    bytes_to_send[3] = 108;
	    bytes_to_send[4] = 111;
	 
	    // Declare variables and structures
	    HANDLE hSerial;
	    HANDLE hFirmware;
	    DCB dcbSerialParams = {0};
	    COMMTIMEOUTS timeouts = {0};
	    if ( argc != 3 ) /* argc should be 2 for correct execution */
	    {
		/* We print argv[0] assuming it is the program name */
		printf( "usage: %s filename com-port #\n", argv[0] );
	    }         
	    else {

	    //nwave edition
	    /*hSerial = CreateFile(
		        "\\\\.\\COM22", GENERIC_READ|GENERIC_WRITE, 0, NULL,
		        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	    */
	#if 0
	    hFirmware = CreateFile(
		        argv[1], GENERIC_READ, 0, NULL,
		        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	    LARGE_INTEGER fw_size;
	#endif
	//	FILE *f = fopen("textfile.txt", "rb");



	FILE *f = fopen(argv[1], "rb");
	fseek(f, 0, SEEK_END);
	long length = ftell(f);
        printf("Nwave bootloader utility.\n");
        printf("File length: %d\n", length);
	fseek(f, 0, SEEK_SET);

	char *buffer = malloc(length + 1);
	fread(buffer, length, 1, f);
	fclose(f);
	/*
	    DWORD fw_res = GetFileSize(
		  hFirmware,
		  fw_size
		);
	    return;
	*/
	/*
	    BOOL var = GetFileSizeEx(
	       hFirmware,
	       &fw_size
	    );
	*/
    

    char com_port_name[] = "\\\\.\\COMXXX";
    //printf("\nfirst_com string:%s\n", com_port_name);
    strcpy ((char *)(com_port_name + 7), argv[2]);
    

    // Open the highest available serial port number
//#if DPRINT > 0
    fprintf(stderr, "Opening serial port...");
//#endif
    /*
    hSerial = CreateFile(
                "\\\\.\\COM4", GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    */
    //printf("\ncom string:%s\n", com_port_name);
    hSerial = CreateFile(
                com_port_name, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );


    if (hSerial == INVALID_HANDLE_VALUE)
    {
            fprintf(stderr, "Error\n");
            return 1;
    }
    else fprintf(stderr, "OK\n");
     
    // Set device parameters (38400 baud, 1 start bit,
    // 1 stop bit, no parity)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error getting device state\n");
        CloseHandle(hSerial);
        return 1;
    }
     
    //nwave edition
    //dcbSerialParams.BaudRate = CBR_38400;
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if(SetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error setting device parameters\n");
        CloseHandle(hSerial);
        return 1;
    }
 
    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if(SetCommTimeouts(hSerial, &timeouts) == 0)
    {
        fprintf(stderr, "Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }
 
    // Send specified text (remaining command line arguments)
    DWORD bytes_written, total_bytes_written = 0;
#if DPRINT > 0
    fprintf(stderr, "Sending bytes...\n");
#endif
//nwave edition
    char an_ack_U[1] = "U";
    char an_ack_u[1] = "u";
    if(!WriteFile(hSerial, an_ack_U, 1, &bytes_written, NULL))
    {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return 1;
    } 
    char ack[1] = {0};
    DWORD bytes_read;
#if DPRINT > 0
    printf("Wait for S-ack\n");
#endif
    while (ack[0] != 0x53) {
     ReadFile(hSerial, ack, 1, &bytes_read, NULL);
     // TODO: Implement programme termination by timeout of 3 seconds.
     // Message "Terminated due to timeout."
    }
#if DPRINT > 0
    printf("S-ack received\n");
#endif
    if(!WriteFile(hSerial, an_ack_u, 1, &bytes_written, NULL))
    {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return 1;
    } 



    if(!WriteFile(hSerial, /*bytes_to_send*/ &length, 4, &bytes_written, NULL))
    {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return 1;
    }
#if DPRINT > 0   
    fprintf(stderr, "%d bytes written\n", bytes_written);
#endif     

    int block_cnt = length / BLOCK_SIZE;
    int tail = length % BLOCK_SIZE;
#if DPRINT > 0
    printf("block_cnt: %d\n", block_cnt);
    printf("tail: %d\n", tail);
#endif
    int cnt = 0;


            // TO DO:// send the len
            for (i = 0; i <= block_cnt - 1; i++)
               {
		   // 1. Send current block to the serial port
                    int j;
                    //for(j = 0; j <= BLOCK_SIZE - 1; j++)
                     //printf("0x%x ", *((unsigned char *)buffer + j) );
#if DPRINT > 0
                   printf("block_number: %i\n",i);
#endif
		    if(!WriteFile(hSerial, (buffer + i * BLOCK_SIZE), BLOCK_SIZE, &bytes_written, NULL))
		    {
			fprintf(stderr, "Error\n");
			CloseHandle(hSerial);
			return 1;
		    }


		    //ssize_t size = read(fd_serial, &byte, 1);
		   //
		   // return 0;
                   // 2. Wait for the return answer form the MCU via this serial port (block accepted confimation
		  unsigned short blck_calc_crc;
                  blck_calc_crc = 0;
		  blck_calc_crc = slow_crc16(0, (buffer + i * BLOCK_SIZE) , BLOCK_SIZE);
#if DPRINT > 0
		  printf ("blck_calc_crc is: 0x%x\n",blck_calc_crc);
		  printf("wait for ack C...");
#endif
		       while (ack[0] != 0x43) {
	   		    //printf("waiting...temp: %d\n", temp);
                            ReadFile(hSerial, ack, 1, &bytes_read, NULL);
			    //temp++;
			}
#if DPRINT > 0
 		  printf("received C.\n");
#endif
		  //write(fd_serial, &blck_calc_crc , 2);
		    if(!WriteFile(hSerial, /*bytes_to_send*/ &blck_calc_crc, 2, &bytes_written, NULL))
		    {
			fprintf(stderr, "Error\n");
			CloseHandle(hSerial);
			return 1;
		    } 


                  ack[0] = 0;
#if DPRINT > 0
		  printf("wait for ack B...");
#endif
		   while (ack[0] != 0x42) 	   {
                        ReadFile(hSerial, ack, 1, &bytes_read, NULL);
		   }
#if DPRINT > 0
   		   printf("received B.\n");
#else
		   fprintf(stderr, ".");
#endif

                   
	       }
            //3. Send the tail to the serial port
#if DPRINT > 0
           printf("Now tail:\n");
#endif
	   ack[0] = 0;

          
	    if(!WriteFile(hSerial, (buffer + i * BLOCK_SIZE ), tail, &bytes_written, NULL))
	    {
		fprintf(stderr, "Error\n");
		CloseHandle(hSerial);
		return 1;
	    }



	   while (ack[0] != 0x42) 	   {
               ReadFile(hSerial, ack, 1, &bytes_read, NULL);
	   }

            //4. TO DO: find where the linker put the CRC (probably last two bytes), read out it from file(buffer)
	   unsigned short calc;
           calc = slow_crc16(0, buffer, length);
#if DPRINT > 0
           printf ("calc of crc is: 0x%x\n",calc);
#endif

           //4(continue of 4). Send this CRC
           unsigned char *calc_ptr = (unsigned char*)&calc;



	    if(!WriteFile(hSerial, calc_ptr, 1, &bytes_written, NULL))
	    {
		fprintf(stderr, "Error\n");
		CloseHandle(hSerial);
		return 1;
	    }
#if DPRINT > 0
           printf ("1st byte sent: x%x\n",*(unsigned char*)calc_ptr);
#endif

	   while (ack[0] != 0x42) 	   {

               ReadFile(hSerial, ack, 1, &bytes_read, NULL);
	   }
           calc_ptr++;
	   if(!WriteFile(hSerial, calc_ptr, 1, &bytes_written, NULL))
	    {
		fprintf(stderr, "Error\n");
		CloseHandle(hSerial);
		return 1;
	    }
#if DPRINT > 0
           printf ("2nd byte sent: x%x\n",*(unsigned char*)calc_ptr);
#endif

           int flag = 1;

	   while (flag == 1) 	   {
               ReadFile(hSerial, ack, 1, &bytes_read, NULL);
               if ((ack[0] == 0x58) || (ack[0] == 0x5A))  flag = 0;
	   }


#if DPRINT > 0
           if (ack[0] == 0x58)  printf("\nTotal CRC failed\n");
           else if (ack[0] == 0x5A)  printf ("\nTotal CRC Succeed\n");
#else
           if (ack[0] == 0x58)  printf("\nFlash upload failed: CRC error\n");
           else if (ack[0] == 0x5A)  printf ("\nFlash upload successfully completed\n");
#endif  

	   unsigned int delay;
           for(delay = 0; delay < 0xFFFFFF; delay++); //wait just a little bit before closing the port\
           //otherwise it might be closed eariler that the last characters are sent

	   if(!WriteFile(hSerial, "r", 1, &bytes_written, NULL)) //Reset MCU
	    {  
		fprintf(stderr, "Error\n");
		CloseHandle(hSerial);
		return 1;
	    }



	    // Close serial port
//#if DPRINT > 0
	    fprintf(stderr, "\nClosing serial port...");
//#endif
	    if (CloseHandle(hSerial) == 0)
	    {
		fprintf(stderr, "Error\n");
		return 1;
	    }
	    fprintf(stderr, "OK\n");
	 
	    // exit normally
	    return 0;
  }
}
