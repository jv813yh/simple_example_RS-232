# simple_example_RS-232
This is my first application using the RS-232 communication channel so I apologize for the stupid mistakes.

Example of the use of the RS-232 communication channel in
language C at 115200 Bd without any protection of transmitted data.

Source code demo_tx.c creates an input file as needed
user or loads the input file specified by the user and sends the size of the transferred file and the size of the sent blocks. The other side (PC2)
receive the size of the transferred file and the size of the transferred blocks.
This is followed by the actual data transfer in blocks. The other party sends a success message resp
not sending data to the first party. The process is repeated accordingly
or quit. COM ports (numbers need to be modified
in source codes).


The program can be compiled using a Makefile. They are added
.bat files for easy running of programs in Windows. For
Linux is required to call programs from the CLI.The programs are portable between Windows and Linux.

I downloaded the RS-232 library:
https://www.teuniz.net/RS-232/

I use the com0com emulator, which can only run on Windows
to create virtual COM ports:
http://com0com.sourceforge.net/

On Linux I use:

Because I didn't want to use any cryptography in the programs
protection of transmitted data or something similar, the file Compare_files.c is added,
which compares two files and evaluates whether they are identical or not:

gcc compare_files.c -o cmp
