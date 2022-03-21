
/**************************************************
file: demo_tx.c
purpose: simple demo that transmits file to
the serial port 

compile with the command: gcc demo_tx.c rs232.c -Wall -Wextra -o2 -o test_tx
or with the make

Author: Jozef Vendel, Date: 24.12.2021

KEMT FEI TUKE, Diploma thesis
**************************************************/

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
/* /dev/ttyS0 (COM1 on windows) port */
#define CPORT_NR                0
/* 115200 baud, bit rate */
#define B_TRATE                 115200

/* sleep for 300 milliSeconds - WINDOWS */
#define MILISECONDS_WIN     500

/* sleep for 300 milliSeconds - LINUX */
#define MILISECONDS_LIN      300000

//Maximum size of data in block
#define INTERNALL_BUFFER 	  3050
//Size of static array
#define STATIC_ARRAY          1024


/*======= Global function implementation ===========*/

/* Function for creating / loading input file */

unsigned char *loading_file(char *file, 
                      uint32_t *file_size, 
                      int my_file);


int main(void)
{

int
      true_count = 0, again = 0,
      sent_bytes = 0,  last_block = 0,
   test = 0, first_block = 0, finish = 0, help_block;

  uint32_t file_size = 0, begin = 0,
  sent_size = INTERNALL_BUFFER; /* Maximum transfer size in block is sent_size */

  unsigned char convert[STATIC_ARRAY], *input;

//8N1 means eight databits, no parity, one stopbit
//If flowctrl is set to 0, no flow control is used
  char mode[]={'8','N','1',0},
      own_file[150];     /**< Own file name */ 

  clock_t start, stop;

  printf("\n***************** File creation / selection *********************\n");
  printf("* Do you want to use a random text file to test the application *\n" 
          "* or you want to use the application according to your needs    *\n"
          "* Press number 0 or 1                                           *\n"
          "* '1' -> to test the application                                *\n"
          "* '0' -> own data                                               *\n");
  printf("*****************************************************************\n");

  scanf("%d", &test);

 /* ========  Loading input data  ================= */
  if (test)
  {  
    printf("\n\nYou have decided to create a test file to test the application\n"
           "follow this instructions:\n"
           "Please enter name of file with suffix, example: example.txt \n");

    scanf("%s", own_file);

    /* Random generate file and loading input data */
    input = loading_file(own_file, &file_size, 0); 

    printf("\nFile size is: %u\n", file_size);

  
  } else 
  {
    printf("You have decided to load your own file\n");
    printf("Please enter name of file with suffix, example: example.txt\n");
    
    scanf("%s", own_file);

    /* Loading input data */
    input = loading_file(own_file, &file_size, 1); 

    printf("\nFile size is: %u\n", file_size);
  }

  //Check if the memory has been successfully
  //allocated by malloc or not
  if (input == NULL)
  {
    printf("Memory not allocated in loading file\n");
    exit(0);
  }


  /* Counting the number of blocks */
  float count_blocks = file_size /(float) INTERNALL_BUFFER;
  count_blocks = ceil(count_blocks);
  fprintf(stdout, "\nThe count of blocks sent will be: %.0f", count_blocks);


  /* Opens the comport */
  if(RS232_OpenComport(CPORT_NR, B_TRATE, mode, 0))
  {
    printf("Can not open comport\n");

    return(0);
  }


 /* Convert int (file size) to char and send it */
  sprintf((char *)convert, "%d", file_size);
  printf("\nSize of data is: %u\n", file_size);
  int annouc = RS232_SendBuf(CPORT_NR, convert, STATIC_ARRAY);
  if(annouc > 0) printf("\nThe size of the transmitted data was sent\n\n");
  
#ifdef _WIN32
      Sleep(MILISECONDS_WIN); /* sleep for 100 milliSeconds - WINDOWS */
#else
      usleep(MILISECONDS_LIN);  
#endif

  /* Sending a loaded file in blocks and waiting for the feedback */
  while (1)
  {

    /* If transmission will be repetead */
    if(again) 
    {
      begin = 0;
      again--;
      first_block = 0;
      finish = 0;
    }
 
    /*If the size of the data being sent is less than INTERNALL_BUFFER */
    if (file_size < sent_size) sent_size = file_size;

    /* Sends multiple bytes via the serial port */
    sent_bytes = RS232_SendBuf(CPORT_NR, input + begin, sent_size);
    
    /* Start data transfer	*/
    if(sent_bytes > 0 && first_block == 0) 
    {
      start = clock();
      first_block++;
    }

    /* Data transmission verification */
    if (sent_bytes < 0) 
    {
        printf("Failed to send data\n");
        printf("Closed sending data and comport\n");
    	break;
    }

    /* Listing the number of bytes and block sent */
    printf("%d. sending: %d bytes\n", true_count++, sent_bytes);

    	begin += sent_bytes;
      printf("begin: %d\n", begin);

#ifdef _WIN32
      Sleep(MILISECONDS_WIN); /* sleep for 100 milliSeconds - WINDOWS */
#else
      usleep(MILISECONDS_LIN);  
#endif
      
      /* If we want to send last less data than size of INTERNAL BUFFER */
      help_block = true_count;
      if (++help_block == count_blocks) last_block++;
      if ((begin % sent_size == 0) && (last_block)
          && (begin != file_size)) sent_size = file_size - begin;

    /* Checking if sending data was successful or no */
    while (begin == file_size && true_count == (int)count_blocks)
    {   
      /* End of data transmission measurement */  
      stop = clock();
      double elapsed = (double)(stop - start)  / CLOCKS_PER_SEC;
      printf("\n****************** Summary *********************\n");
      printf("File transfer about size: %u time took seconds: %f\n", file_size, elapsed);

      int n = 0, result = 0, ok = 0;
      unsigned char buf[STATIC_ARRAY];

      /* Report on the expected success / failure of the data transmission */

      do 
      {
          n = RS232_PollComport(CPORT_NR, buf, STATIC_ARRAY);
          if (n > 0)
          {
            printf("\n\nReceiving: %d bytes\n%.*s\n", n, n, (char *)buf);
            result = memcmp(buf, "Sending of data was successful :)\n", n);
            ok++;
          }
        } while (n == 34 || n == 68);


      if (result == 0 && ok == 1) 
      {
        finish++;
        break;
      }
      else if (ok == 1)
      { 
        memset(buf, 0, n);
        again++;
        break;
      }
    }

    /* Checking, termination or continuation*/
    if (finish) break; 
  }

  /* ========  End of application  ========================= */

  printf("Finish...\n");
  printf("Closed RS232...\n");

  /* Free memory aloccated */
  free(input);

  /* Closes the serial port */
  RS232_CloseComport(CPORT_NR); 

  return 0;
}


/* Function for creating / loading file */

unsigned char *loading_file(char *file, 
                      uint32_t *file_size, 
                      int my_file)
{   

    FILE *stream;

    int result1, result2;

    unsigned char *input;
    uint32_t expected_size_file,
             range;

    /**
     * if my_file == 0 -> test file 
     * if my_file == 1 -> your file 
     * 
     * Creating our random test file:
     * 
     */

    if(!my_file) 
    {
        if ((stream = fopen(file, "wb")) == NULL) 
        {
            printf("Failed to open create file %s\n", file);
            exit(0);
        }
        
        printf("Creating own file\n");
      
        uint32_t i = 0;

        printf("Enter the approximate file size in bytes: \n");
        scanf("%u",&expected_size_file);
        expected_size_file = expected_size_file / 
                            (sizeof(expected_size_file) * sizeof(expected_size_file));
        printf("Enter max integer (range): \n");
        scanf("%u",&range);

        while(i++ < expected_size_file)
        {
            fprintf(stream, "Number %d. %u, ", i,  rand() % range);
        }

        fprintf(stream, "\nThis is the end of the file being tested :)");

        if(fclose(stream) == EOF) 
            printf("Failed to closed file\n");

    }

    if ((stream = fopen(file, "rb")) == NULL) 
    {
        printf("Failed to open file %s\n", file);
        exit(0);
    }

    /**
     * _fseeki64 functions moves the file pointer (if any) 
     * associated with stream to a new location that is offset 
     * bytes from origin 
     *
     * SEEK_END : End of file
     * SEEK_SET : Beginning of file.
     */

    /* If successful, return 0 */
    if ((result1 = _fseeki64(stream, 0L, SEEK_END)) != 0)
        printf("_fseeki64 error1\n");

    /* _ftelli64 return the current file position */
    *file_size = _ftelli64(stream);

    /* If successful, return 0 */
    if ((result2 = _fseeki64(stream, 0L, SEEK_SET)) != 0)
        printf("_fseeki64 error2\n");

    /* Allocates the requested memory and returns a pointer to it */
    input = (unsigned char *) malloc(*file_size);
    if (input == NULL) 
    {
        printf("Memory not allocated.\n");
        exit(0);
    }

    /*
     * reads data from the given stream into the 
     * array pointed to, by input
     * 
     * file_size is the number of elements, 
     * each one with a size of size bytes.
     */
    fread(input, 1, *file_size, stream);


    if(fclose(stream) == EOF) 
        printf("Failed to closed file\n");

    return input;
}
