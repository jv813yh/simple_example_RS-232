
/**************************************************
* file: demo_tx.c             v.1.2
* purpose: simple demo that transmits file to
* the serial port 
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
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

/* ===== RS-232 local macro definition & library ===== */

/* RS232 library */
#include "rs232.h"
/* Created source codes for rs232.h */
#include "example_modified_rs232.h"
/* /dev/ttyS0 (COM1 on windows) port */
#define CPORT_NR                0
/* 115200 baud, bit rate */
#define B_TRATE                 115200
/* Sleep for 50 milliSeconds */
#define MILISECONDS           50
/* Maximum size of data in block */
#define INTERNALL_BUFFER 	  4094
/* Auxiliary size of static array */
#define STATIC_ARRAY          1024


int main(void)
{
    /* 
     * file_size -> The size of the transferred file.
     * finish -> Verification of the transmission, 
     *           whether it succeeded or not. If no 
     *           transmission must be reperated.
     *
     * test ->   Select your own or created file.
     * data_transfer -> Return value determines whether 
     *          the data transfer 
     *          transmission was successful or not
     */
    uint32_t file_size = 0, finish = 1, test,
             data_transfer = 0, sleep; 
 
    /* Buffer for loading the file being sent */
    unsigned char *input;

    //8N1 means eight databits, no parity, one stopbit
    //If flowctrl is set to 0, no flow control is used
    char mode[]={'8','N','1',0},
         own_file[150]; /*<- Own file name */ 

    printf("\n***************** File creation / selection *********************\n");
    printf("* Do you want to use a random text file to test the application *\n" 
          "* or you want to use the application according to your needs    *\n"
          "* Press number 0 or 1                                           *\n"
          "* '1' -> to test the application                                *\n"
          "* '0' -> own data                                               *\n");
    printf("*****************************************************************\n");
    if (EOF == scanf("%d", &test))
    {
        printf("Oh no man :( bad choice, the program will end.\nPlease turn it on again\n");
        return -1;
    } 

/* ========  Loading input data  ================= */
    if (test)
    {  
        printf("\nYou have decided to create a test file to test the application.\n"
            "Follow this instructions:\n"
            "Please enter name of file with suffix, example: example.txt\n");
    if (EOF == scanf("%s", own_file))
    {
        printf("Oh no man :( bad choice, the program will end.\nPlease turn it on again\n");
        return -1;
    }

    /* Random generate file and loading input data */
    input = loading_file(own_file, &file_size, 0); 
    } else
    {
        printf("\nYou have decided to load your own file.\n");
        printf("Please enter name of file with suffix, example: example.txt\n");
        if (EOF == scanf("%s", own_file))
        {
            printf("Oh no man :( bad choice, the program will end.\nPlease turn it on again\n");
            return -1;
        }

        /* Loading input data */
        input = loading_file(own_file, &file_size, 1); 
    } 

    /* Opens the comport */
    if(RS232_OpenComport(CPORT_NR, B_TRATE, mode, 0))
    {
        printf("Can not open comport\n");
        return -1;
    }

/* ========  Data transmission ================= */
  /* 
   * All data transmission and transmission verification or retransmission
   * is performed in this loop -> while(finish){....}
   */
    while(finish)
    {   
        data_transfer = send_data_blocks(CPORT_NR,
                                        input,
                                        file_size,
                                        INTERNALL_BUFFER,
                                        MILISECONDS);
        if (data_transfer != 1)
        {
            printf("There was a problem with sending data, you need to start over\n");
            return -1;
        } else 
        {
            printf("\nI'm waiting for a transmission report !!!\n");
            /* Checking if sending data was successful or no */
            int32_t n = 0, result = 0, ok = 0;
            unsigned char buf[STATIC_ARRAY];
            /* Report on the expected success / failure of the data transmission */
            while(1)
            {
                n = RS232_PollComport(CPORT_NR, buf, STATIC_ARRAY);
                if (n < 0)
   	            {
      	    	    printf("Incorrect data received\n");
      		        return -1;
    	        }  
                if (n > 0)
                {
                    printf("\nReceiving: %d bytes\n%.*s\n", n, n, (char *)buf);
                    result = memcmp(buf, "Success", n);
                    ok++;
                }
                if (result == 0 && ok == 1)
                {   
                    printf("\n");
                    /* Ok, We can stop sending data */
                    finish--;

                    break;
                } else if (ok == 1)
                {
                    printf("\nFailure, transmission must be repeated\n");
                    memset(buf, 0, n);

                    break;
                }

                /* 
                 * Suspends the implementation of the program for a 
                 * specified number of seconds
                 */
                if ((sleep = sleep_miliseconds_win_linux(MILISECONDS)) == 0)
                {
                    printf("Problem during sleep");
                    return -1;
                }
            } /* End of report on the expected success/failure of the data transmission */
        } /* End of else for transmission evaluation */
    } /* End of data transfer */

/* ========  End of application  ========================= */

  printf("Finish...\n");
  printf("Closed RS232...\n");

  /* Free memory aloccated */
  free(input);

  /* Closes the serial port */
  RS232_CloseComport(CPORT_NR); 

  return 0;
}


