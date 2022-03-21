
/**************************************************
file: demo_rx.c
purpose: simple demo that read data from 
serial port and print them on the screen

compile with the command: gcc demo_tx.c rs232.c -Wall -Wextra -o2 -o test_tx
or with the make

Author: Jozef Vendel, Date: 24.12.2021

KEMT FEI TUKE, Diploma thesis
**************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

//RS232 library
#include "rs232.h"

//Maximum size of data in block
#define INTERNALL_BUFFER      4000
//Size of static array
#define STATIC_ARRAY          1024

#define INACTIVE                  5


int main(void)
{
  int received_bytes = 1,
      begin = 0, true_count = 0,
      only_size = 0, k = 1,
      expected_size = 0,
      cport_nr = 1,        /* /dev/ttyS0 (COM1 on windows) */
      bdrate = 115200;       /* 115200 baud */

      int idle = 0;

  float count_blocks;

//8N1 means eight databits, no parity, one stopbit
//If flowctrl is set to 0, no flow control is used
  char mode[]={'8','N','1',0}, check_data[STATIC_ARRAY];
  unsigned char buf[INTERNALL_BUFFER], size_file[STATIC_ARRAY];

//Opens the comport
  if(RS232_OpenComport(cport_nr, bdrate, mode, 0))
 	{
    printf("Can not open comport\n");
	}

/////Je mozne upravit prijem a odosielanie dat do funkcii/funkcie......
//Accepting the expected data size and calculate the number of blocks  
  while (1) 
  {  
//Expected size of data
    only_size = RS232_PollComport(cport_nr, size_file, STATIC_ARRAY);

//Convert expected size of data from unsighed char (size_file)
//to int (expected_size)reading
      if (only_size > 0 ) sscanf((const char *)size_file, "%d", &expected_size);

      if (expected_size > 0) 
      {
          printf("\nExpected size of data is: %d\n", expected_size);
//Counting the number of blocks
          count_blocks = expected_size /(float) INTERNALL_BUFFER;
          count_blocks = ceil(count_blocks);


#ifdef _WIN32
      Sleep(100);
#else
      usleep(1000000); 
#endif
          break;
      }
    }

    int first_block = 0;

//Receiving data
  while(1)
  {
    int finish = 0;
    
//Opens the file received_data.txt
    FILE *fp;

    fp = fopen("received_data.txt", "wb");

    if(fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

/*
    /* If transmission will be repetead 
    if(again) 
    {
      begin = 0;
      again--;
      first_block = 0;
      finish = 0;
    }
    */

do {


//Gets characters from the serial port  	
    received_bytes = RS232_PollComport(cport_nr, buf, INTERNALL_BUFFER);

    if(received_bytes > 0 && first_block == 1)
    {
        fwrite(buf, 1, received_bytes, fp);
        printf("%d.receiving: %i bytes\n", k++, received_bytes);
    //  printf("received data:\n%.*s\n", received_bytes, (char *)buf);

//Addition received bytes
        begin += received_bytes;
        true_count++;

        idle = 0;

    } else if  (received_bytes > 0 && first_block == 0) 
    {
        first_block++;
    } else if (received_bytes < 0) 
    {
      printf("Failed to send data\n");
      printf("Closed sending data and comport\n");
      break;
    }
    printf("begin: %d , expected_size: %d \n", begin, expected_size);

/*
    if (received_bytes == 0)
    {
        printf("received_bytes: %u\n", received_bytes);
        printf("idle: %d\n", idle);
        idle++;
    }
    */

/*
    if (idle == INACTIVE)
        break;
*/

#ifdef _WIN32
      Sleep(100);
#else
      usleep(1000000); 
#endif

  } while(begin != expected_size);

//Check if wass all data has been sent, if yes 
//we send data that sending of data is OKEY and break of procces
//or no,  we send that sending data must be send again
    if (begin == expected_size)
      {
        strcpy(check_data, "Sending of data was successful :)\n");
        printf("%s\n\n", check_data);
        int length = strlen(check_data);
        int ok_message;
        ok_message = RS232_SendBuf(cport_nr, (unsigned char *) check_data, length);
        printf("Sent %d bytes about confirming the report\n\n", ok_message);
        memset(check_data, 0, length);

        finish++;
      } else 
      {
         strcpy(check_data, "Sending of data was not successful, you must send all data again :/\n");
         printf("\n%sThe transmission will be repeated.\n", check_data);
         int length1 = strlen(check_data);
         int ok_message;
         ok_message = RS232_SendBuf(cport_nr, (unsigned char *)check_data, length1);
         printf("Sent %d bytes about confirming the report\n", ok_message);
         memset(check_data, 0, length1);
#ifdef _WIN32
      Sleep(100);
#else
      usleep(1000000);  /* sleep for  1 Seconds */
#endif
      }

      if(finish) break;

//Closed open file
    fclose(fp); 
  }

  printf("Finish...\n");
  printf("Closed RS232...\n");

//Closes the serial port 
  RS232_CloseComport(cport_nr);
  return 0;
}

