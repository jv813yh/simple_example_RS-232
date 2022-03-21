# simple_example_RS-232
This is my first application using the RS-232 communication channel so I apologize for the stupid mistakes.

Demonstration of an example using the RS-232 communication channel in C language without data protection. The test_tx.exe application loads the input file specified as the second argument when calling test_tx, find out its size, which is the first message sends to the other party. It will receive the management of the size of the expected data and access The other party sends a success report or failure to send data to the first party. Accordingly, the process is repeated or quit. COM ports (numbers need to be modified in source codes). Data is sent in blocks of constant size INTERNAlL_BUFFER !!! The recipient writes data to the CLI (or file received_data.txt., I will add this option to the next version)

/* I will add this option to the next version / The compile code comapre_files.c is added and can be compiled using the command: gcc compare_files.c -o cmp_files With this program it is possible to check sending and receiving file is identical. / */

The program can be compiled using the Makefile file. Text files serve as a basis for application demonstration. They are added .bat files for easy program execution.

Windows/Linux
