
/**************************************************
* file: demo_rx.c             v.1.1
* purpose: simple demo that read data from 
* serial port and stores them in a file
*
* Compile with the makefile
*
* Windows / Linux
* 
* Author: Jozef Vendel, Create date: 24.12.2021
*
* KEMT FEI TUKE, Diploma thesis
**************************************************/

/* Basic libraries for working in C. */
#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

/* ===== RS-232 local macro definition & library ===== */
#include "rs232.h"
/* Created source codes for rs232.h */
#include "example_modified_rs232.h"
/* /dev/ttyS0 (COM1 on windows) port */
#define CPORT_NR                1
/* 115200 baud, bit rate */
#define B_TRATE                 115200
/* Sleep for 50 milliSeconds  */
#define MILISECONDS             50
/* Auxiliary size of static array */
#define STATIC_ARRAY            1024


int main(void)
{
    /* 
     * Return value determines whether the data transfer 
     * transmission was successful or not
     */
    uint32_t result_transmission = 0, finish = 0, sleep_return;

    /* 
     * 8N1 means eight databits, no parity, one stopbit
     * If flowctrl is set to 0, no flow control is used
     *
     * check_data -> Buffer for confirmin transmission report
     */
    char mode[]={'8','N','1',0}, check_data[STATIC_ARRAY];

    /* Opens the comport */
    if(RS232_OpenComport(CPORT_NR, B_TRATE, mode, 0))
 	{
        printf("Can not open comport\n");
        return -1;
	}

/* ========  Data transmission ================= */
    do
    { 
        /* All data transfer is performed by this function */
        result_transmission = data_receiving_blocks(CPORT_NR,
                                                    MILISECONDS);
        if (result_transmission != 1)
        {
            printf("Data reception failure\n");
            return -1;
        }
	/* 
         * Suspends the implementation of the program for a 
         * specified number of seconds
         */
         if ((sleep_return = sleep_miliseconds_win_linux(MILISECONDS * 2)) == 0)
         {
             printf("Problem during sleep");
             return -1;
         }
        /* 
         * Check if wass all data has been sent, if yes 
         * we send data that sending of data is OKEY and break of procces
         * or no,  we send that sending data must be send again
         */
        if (result_transmission)
        {
            strcpy(check_data, "Success");
            printf("%s\n", check_data);
            int length = strlen(check_data);
            int ok_message;
            ok_message = RS232_SendBuf(CPORT_NR, (unsigned char *) check_data, length);
	    if (ok_message < 0)
   	    {
      	    	printf("Incorrect data send\n");
      		return -1;
    	     }  
            printf("Sent %d bytes about confirming the report\n\n", ok_message);
        } else 
        {
            strcpy(check_data, "Fail");
            printf("\nThe transmission will be repeated.\n");
            int length1 = strlen(check_data);
            int ok_message;
            ok_message = RS232_SendBuf(CPORT_NR, (unsigned char *)check_data, length1);
	    if (ok_message < 0)
   	    {
      	    	printf("Incorrect data send\n");
      		return -1;
    	     }  
            printf("Sent %d bytes about confirming the report\n", ok_message);
            
            finish++;
            memset(check_data, 0, length1);
        }
        /* 
         * Suspends the implementation of the program for a 
         * specified number of seconds
         */
         if ((sleep_return = sleep_miliseconds_win_linux(MILISECONDS)) == 0)
         {
          	printf("Problem during sleep");
                return -1;
          }
    } while (finish);

  printf("Finish...\n");
  printf("Closed RS232...\n");

 /*Closes the serial port */
  RS232_CloseComport(CPORT_NR);
  return 0;
}

