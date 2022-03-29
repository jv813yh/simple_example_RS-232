##################################################
## Diplomova  praca                             ##
## Meno studenta: Jozef Vendel                  ##
## Veduci DP: prof. Ing. Milos Drutarovsky CSc. ##
## Skola: KEMT FEI TUKE                         ##
## Datum vytvorenia: 24.12.2021	                ##
##################################################

Zoznam suborov
  
 simple_example_RS-232
      |__INC
      |__SRC_LIB
      |__compare_files
      |__demo_rx.bat
      |__demo_rx.c
      |__demo_tx.c
      |__demo_tx.bat
      |__Makefile
      |__readme.txt


Demonstrovanie prikladu vyuzivajuceho komunikacny kanal RS-232 v
jazyku C pri 115200 Bd bez ziadnej ochrany prenasanych dat. 

Aplikacia demo_tx.exe vytvori vstupny subor podla poziadaviek
uzivatela alebo nacita vstupny subor zadany uzivatelom. Druhej strane
posle velkost prenasaneho suboru a velkost prenasanych blokoch, kedze data
sa prenasaju v blokoch. Druha strana predstavuje PC2(demo_tx.c) prijme spravu velkosti ocakavanych dat a velkosti prenasanych blokch a pristupi
sa k odosielanu samotnych dat.Druha strana odosle spravu o uspenosti alebo
neuspenosti odosielania dat prvej strane. Podla toho sa proces opakuje
alebo ukonci. Vyuzivaju sa COM porty (cisla, je potrebne upravit
v zdrojokoych kodoch).

Program je kompilovatelny pomocou Makefile suboru. Su dodane
.bat subory pre jednoduche spustenie programov na Windows. Pre
Linux je potrebne zavolat programy z CLI.

Programy su prenositelne medzi Windows a Linux.

Kniznicu RS-232 som stiahol: 
https://www.teuniz.net/RS-232/

Vyuzivam emulator com0com, ktory je spustitelny len na Windows
pre vytvorenie virtualnych COM portov:
http://com0com.sourceforge.net/

Na Linuxe vyuzivam:

Kedze v programoch som nechcel vyuzivat ziadnu kryptograficku
ochranu prenasanych dat ani nic podobne, je pridany subor compare_files.c,
ktory porovnava dva subory a vyhodnoti, ci su identicke, alebo nie.
gcc compare_files.c -o cmp

