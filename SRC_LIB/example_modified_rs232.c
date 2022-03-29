/**
 * ===============================================
 * example_modified_rs232c   v.0.2
 * 
 * KEMT FEI TUKE, Diploma thesis
 *
 * Created auxiliary functions for working with RS-232.
 * The functions are used for transfer between two PCs, 
 * where PC1 reads or creates a file and sends it in 
 * blocks to PC2. The whole process will be performed 
 * and according to the evaluation it will either end 
 * (in case of success) or it will be repeated until 
 * it is successful.
 *
 * Works with functions I demonstrate in source codes 
 * demo_tx.c an demo_rx.c
 * 
 * Windows / Linux
 * 
 * Author-Jozef Vendel  Create Date- 28.03.2022 
 * ===============================================
 */

/* Basic libraries for working in C. */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* ===== RS-232 local macro definition & libraries ===== */
#include "rs232.h" /* RS-232 channel library */
#include "example_modified_rs232.h" /* Created functions for work */
/*
 * The os (kernel) has an internal buffer of 4096 bytes.
 * If this buffer is full and a new character arrives on the serial port,
 * the oldest character in the buffer will be overwritten and thus will be lost.
 *
 */
#define INTERNALL_BUFFER      4096
/* Auxiliary size of static array */
#define STATIC_ARRAY          1024

uint32_t expected_size_data(int cport_number, 
                            int sleep_miliseconds,
                            int change)
 { 

    /* Buffer for receive expectede size */
    unsigned char size_file[STATIC_ARRAY];
    memset(size_file, 0, sizeof(size_file));

    /* Return value is in the expected_size */
    uint32_t only_size = 0, expected_size, sleep;

    while (!only_size) 
    {  
        /* Expected size */
        only_size = RS232_PollComport(cport_number, size_file, sizeof(size_file));

        //Convert expected size  from unsighed char 
        //to int (expected_size)reading
        if (only_size > 0 ) sscanf((const char *)size_file, "%u", &expected_size);

        if (expected_size > 0 && only_size > 0) 
        {
            if(!change)  printf("\nExpected size of data is: %d\n", expected_size);
            else printf("\nExpected size of block is: %d\n", expected_size);

            /* 
             * Suspends the implementation of the program for a 
             * specified number of seconds
             */
          if ((sleep = sleep_miliseconds_win_linux(sleep_miliseconds)) == 0)
          {
            printf("Problem during sleep");
            return -1;
          }
        } /* End of excpeted_size > 0 */
    }

    return expected_size;
}

uint32_t data_receiving_blocks(int cport_number, 
                              int sleep_miliseconds)
{   
    /*
     * begin ->               Addition of received data
     * received_bytes ->      Currently received number of bytes
     * expected_size ->       Expected file size
     * expected_size_block -> Expected block size
     * last_block_size ->     The size of the last block
     * entire_block ->        Confirmation of receipt of the entire block
     * true_count_block ->    Expected number of blocks
     * sleep ->               Return value for sleep()
     */
    uint32_t begin = 0, received_bytes = 0,  expected_size = 0,
             expected_size_block = 0, last_block_size = 0, first_block = 0,
             entire_block = 0, true_count_block = 0, sleep;

    /* Data transfer is measured */
    clock_t start, stop;

    /* Number of blocks */
    float count_blocks;
    /* Buffer for receiving data in blocks */
    unsigned char buf[INTERNALL_BUFFER];

    /* Opens the file received_data.txt */
    FILE *fp;

    fp = fopen("received_data.txt", "wb");

    if(fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    /* Receiving the size of the transferred file */
    expected_size = expected_size_data(cport_number,sleep_miliseconds, 0);
    if (expected_size <= 0)
    {
        printf("Bad file size.\nPlease turn it on again\n");
        return -1;
    }

    /* Receiving the size of block  */
    expected_size_block = expected_size_data(cport_number,sleep_miliseconds, 1);
    if (expected_size_block > 0) 
    {
        /* Counting the number of blocks */
        count_blocks = expected_size /(float) expected_size_block;
        last_block_size = expected_size % expected_size_block;
        count_blocks = floor(count_blocks);

        printf("The number of blocks will be %.0f\n", count_blocks);
       /* 
        * Suspends the implementation of the program for a 
        * specified number of seconds
        */
        if ((sleep = sleep_miliseconds_win_linux(sleep_miliseconds)) == 0)
        {
            printf("Problem during sleep");
            return -1;
        }
    } else 
    {
        printf("Bad block size.\nPlease turn it on again\n");
        return -1;
    }

    /* Data reception */
    do
    {
        /* Gets characters from the serial port */
        received_bytes = RS232_PollComport(cport_number, buf, expected_size_block);

        if (received_bytes > 0 && first_block == 0)
        {
            /* Start of transmission measurement */
            start = clock();
            first_block++;
        }
        /* My guard in case of the current receiving of the big block, due to buffer overflow */
        if (received_bytes > 4000) sleep_miliseconds_win_linux(sleep_miliseconds);

        if(received_bytes > 0 && first_block != 0)
        {
            /* Writing the bytes received to the file */
            fwrite(buf, 1, received_bytes, fp);

            printf("%u.receiving: %u bytes\n", first_block++, received_bytes);

            /* Addition received bytes */
            begin += received_bytes;
            entire_block += received_bytes;

            /* End of data transmission measurement */  
            if (begin == expected_size) stop = clock();
            /* 
             * Suspends the implementation of the program for a 
             * specified number of seconds
             */
            /*
             * It is possible to put the process to sleep after each data reception 
             * or after each entire block received, this is after each data reception:

            if ((sleep = sleep_miliseconds_win_linux(sleep_miliseconds)) == 0)
            {
                printf("Problem during sleep");
                return -1;
            }
            */
        }

        /* If the last block remains */
        if (count_blocks == true_count_block) expected_size_block = last_block_size;
          
        /* We have received the whole block of data, we inform the sender */
        if (entire_block == expected_size_block)
        {
            RS232_cputs(cport_number, "OK");
            printf("The entire data block has been received\n");
            true_count_block++;
            entire_block = 0;
              
            /* 
             * Suspends the implementation of the program for a 
             * specified number of seconds.
             * This is after entire block reception:
             */
            if ((sleep = sleep_miliseconds_win_linux(sleep_miliseconds)) == 0)
            {
                printf("Problem during sleep");
                return -1;
            }
        } /* End of entire block */
    } while(begin != expected_size); /* End of data reception */

    double elapsed = (double)(stop - start)  / CLOCKS_PER_SEC;
    printf("\n****************** Summary *********************\n");
    printf("File transfer about size: %u time took seconds: %0.f\n\n", expected_size, elapsed);

    if ((sleep = sleep_miliseconds_win_linux(sleep_miliseconds)) == 0)
    {
        printf("Problem during sleep");
        return -1;
    }

    /* Closed open file */
    fclose(fp); 

    return 1;
}

uint32_t sleep_miliseconds_win_linux(int sleep_miliseconds)
{ 

#ifdef _WIN32
    Sleep(sleep_miliseconds); /* Sleep for sleep_miliseconds - WINDOWS */
#else
    usleep(sleep_miliseconds * 100);  /* - Linux */
#endif

    return 1;
}


unsigned char *loading_file(char *file, 
                      uint32_t *file_size, 
                      int my_file)
{   

    FILE *stream;

    int32_t result1, result2;

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

uint32_t convert_file_and_size_send(int cport_number, 
                                    uint32_t file_size,     
                                    int sleep_miliseconds, 
                                    int change)
{ 
    uint32_t sleep;
    /* Buffer for conversion size to char */
    unsigned char convert_array[STATIC_ARRAY];
    memset(convert_array, 0, sizeof(convert_array));

    /* Convert uint32_t (file_size) to char and send it with cport_number */
    sprintf((char *)convert_array, "%u", file_size);
    if(!change) printf("\nThe size of data is: %u\n", file_size);
    else printf("\nThe size of block is: %u\n", file_size);

    int annouc = RS232_SendBuf(cport_number, convert_array, strlen((char *)convert_array));
    if(annouc > 0 && change == 0) printf("\nThe size of the transmitted data was sent\n\n");
    else if(annouc > 0 && change == 1) printf("\nThe size of the transmitted blocks has been sent\n\n");
  
    /* 
     * Suspends the implementation of the program for a 
     * specified number of seconds
     */
    if ((sleep = sleep_miliseconds_win_linux(sleep_miliseconds)) == 0)
    {
        printf("Problem during sleep");
        return -1;
    }

    return (annouc > 0) ? annouc : 0;
}

uint32_t send_data_blocks(int cport_number, 
                          uint8_t *p_input,
                          uint32_t file_size, 
                          uint32_t block_size, 
                          int sleep_miliseconds)
{
    /*
     * begin ->         Addition of bytes sent
     * sent_size ->     The size of the block being sent
     * sent_bytes ->    Return value sending bytes
     * block_number ->  Block count counter
     * confirm_msg ->   Confirming the receipt of the whole block
     *
     */
    uint32_t begin = 0, first_block = 0, sent_size = 0,
             sent_bytes = 0, block_number = 0, confirm_msg = 0, sleep;

    sent_size = block_size;

    /* Number of blocks sent */
    float count_blocks = file_size / (float) block_size;

    /* Buffer for confirm message for receiving entire block */
    unsigned char buf_confirm_msg[STATIC_ARRAY];

    /* Data transfer is measured */
    clock_t start, stop;

    /* Converts the data size to char and sends it to the recipient */
    uint32_t send_size_of_data = convert_file_and_size_send(cport_number,
                                                    file_size,
                                                    sleep_miliseconds * 4,
                                                    0);
    if (send_size_of_data == 0)
    {
        printf("The size of the transmitted data was not sent !\n");
        return -1;
    }
    /* Sending the size of transferred blocks */
    send_size_of_data = convert_file_and_size_send(cport_number,
                                                 block_size,
                                                 sleep_miliseconds * 4,
                                                 1);
    if (send_size_of_data == 0)
    {
        printf("The size of the transmitted data was not sent !\n");
        return -1;
    }

    /* The data transfer itself in blocks */
    do 
    {
        /* If the size of the data being sent is less than INTERNALL_BUFFER */
        if (file_size < sent_size) sent_size = file_size;

        /* If we want to send last less data than size of INTERNAL BUFFER */
        if (begin + sent_size > file_size) sent_size = file_size - begin;

        if (!first_block)
        {
            /* Start of transmission measurement */
            start = clock();
            first_block++;
        }

        /* Sends multiple bytes via the serial port */
        sent_bytes = RS232_SendBuf(cport_number, p_input + begin, sent_size);
      
        /* Listing the number of bytes and block sent */
        if(sent_bytes > 0) printf("%u. sending: %u bytes\n", ++block_number, sent_bytes);

        /* Addition of bytes sent*/
        begin += sent_bytes;

        /* End of data transmission measurement */  
        if (begin == file_size) stop = clock();

        /* 
         * Suspends the implementation of the program for a 
         * specified number of seconds
         */
        if ((sleep = sleep_miliseconds_win_linux(sleep_miliseconds)) == 0)
        {
            printf("Problem during sleep");
            return -1;
        }

        /* I am waiting for a confirmation message */
        while(sent_bytes > 0) 
        {   
            confirm_msg = RS232_PollComport(cport_number, buf_confirm_msg, sizeof(buf_confirm_msg));
            if (confirm_msg > 0)
            {
                printf("The recipient received %u block from %0.f blocks\n", block_number, count_blocks);
                printf("Number of bytes received %u from %u\n", begin, file_size);
                /* 
                 * Suspends the implementation of the program for a 
                 * specified number of seconds
                 */
                if ((sleep = sleep_miliseconds_win_linux(sleep_miliseconds)) == 0)
                {
                    printf("Problem during sleep");
                    return -1;
                }

                sent_bytes = 0;
            } /* End of -> if (confirm_msg > 0){...} */ 
        } /* End of waiting for confirmation message, while(){...} */
    } while(begin != file_size); /* End of data sending */

    double elapsed = (double)(stop - start)  / CLOCKS_PER_SEC;
    printf("\n****************** Summary *********************\n");
    printf("File transfer about size: %u time took seconds: %0.f\n", file_size, elapsed);

    return 1;
}


