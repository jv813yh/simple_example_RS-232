/* 
 * @file example_modified_rs232.h 	v.0.1
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
 * Windows/Linux  
 *
 * Author-Jozef Vendel  Create Date- 28.03.2022 
 */

#ifndef example_modified_rs232_H
#define example_modified_rs232_H

/* 
 * The sleep() method, which suspends the implementation of the program
 * for a specified number of seconds.
 *
 * @param  sleep_milisecond   Number of milliseconds
 * @return 1		      In case of success
 */
uint32_t sleep_miliseconds_win_linux(int sleep_miliseconds);

/*
 * Accepts the expected amount of transferred data.
 *
 * @param cport_number	      RS232 port number
 * @param sleep_miliseconds   Number of milliseconds
 * @param change 	      0 -> file size
 * 		              1 -> block size
 *
 * @return  		     Expected amount of transferred data
 */
uint32_t expected_size_data(int cport_number, 
                            int sleep_miliseconds,
                            int change);

/*
 * Functions for receiving the size of the expected data, 
 * the size of the transferred blocks and receiving the blocks
 * themselves in the cycle.
 *
 * @param cport_number	      RS232 port number
 * @param sleep_miliseconds   Number of milliseconds
 *
 * @return 1		      In case of success
 */
uint32_t data_receiving_blocks(int cport_number, 
                              int sleep_miliseconds);
/*
 * Function for creating / loading input file. 
 * @return  pointer to the stream(file)
 */

unsigned char *loading_file(char *file, 
                      uint32_t *file_size, 
                      int my_file);
/* 
 * Size conversion to char and send via RS-232.
 *
 * @param cport_number      RS-232 port
 * @param file_size         The size with which it is worked
 * @param sleep_milisecond  After each send, 
 *                          I put the process to sleep in milliseconds.
 * @param change            If change -> 0 -> size of data ...
 *                          If change -> 1 -> size of block ...
 *
 * @return size of bytes sent -> In case of success
 * @return 0                  -> In case of failure
 */
uint32_t convert_file_and_size_send(int cport_number, 
                                    uint32_t file_size, 
                                    int sleep_miliseconds,
                                    int change);

/* 
 * Sending and loaded file in blocks with RS-232.
 *
 * @param cport_number      RS-232 port
 * @param p_input           inpurt buffer
 * @param file_size         The size with which it is worked
 * @param block_size        The size of the block being sent
 * @param sleep_milisecond  After each send, 
 *                          I put the process to sleep in milliseconds.
 *
 * @return 1                -> In case of success
 */
uint32_t send_data_blocks(int cport_number, 
                          uint8_t *p_input,
                          uint32_t file_size, 
                          uint32_t block_size, 
                          int sleep_miliseconds);
#endif


