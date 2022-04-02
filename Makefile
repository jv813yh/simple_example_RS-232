##################################################
## Diplomova praca                              ##
## Meno studenta: Jozef Vendel                  ##
## Veduci DP: prof. Ing. Milos Drutarovsky CSc. ##
## Skola: KEMT FEI TUKE                         ##
## Datum vytvorenia: 29.03.2022	                ##
##################################################

CC=gcc
CFLAGS=-c -O2 -Wall -fcommon -I./INC
LDFLAGS= -lm

#meno vytvorenej kniznice
LIBRARY=modified_rs232.a
#umiestnenie zdrojakov kniznice
SRC_LIB_DIR=SRC_LIB

#automateicke generovanie zdrojakov kniznice
SRC_LIB := $(wildcard $(SRC_LIB_DIR)/*.c)
OBJ_LIB=$(SRC_LIB:.c=.o)

#meno vykonatelneho programu
EXECUTABLE= demo_rx demo_tx
#vymenovanie zdrojakov aplikacie
SRC_EXE=demo_rx.c demo_tx.c
OBJ_EXE=$(SRC_EXE:.c=.o)

all: $(SRC_EXE) $(SRC_LIB) $(EXECUTABLE)

$(EXECUTABLE) : %: %.o $(LIBRARY)
	$(CC) -o $@ $+ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

$(LIBRARY): $(OBJ_LIB) #linkovanie suborov kniznice do statickej kniznice
	ar rcu $@ $+
	ranlib $@

clean:
	rm -f $(EXECUTABLE).exe *.o *.a SRC_LIB/*.o

