##################################################
## Diplomova  praca                             ##
## Meno studenta: Jozef Vendel                  ##
## Veduci DP: prof. Ing. Milos Drutarovsky CSc. ##
## Skola: KEMT FEI TUKE                         ##
## Datum vytvorenia: 24.12.2021	                ##
##################################################

Zoznam suborov
  
 example_rs232
      |__10.txt
      |__5.txt
      |__TEST.txt
      |__TEST1.txt
      |__demo_rx.c
      |__demo_tx.c
      |__Makefile
      |__rs232.c
      |__rs232.h
      |__readme.txt
      |__test_rx.bat
      |__test_tx.bat


Demonstrovanie prikladu vyuzivajuceho komunikacny kanal RS-232 v
jazyku C bez ochrany prenasanych dat. 
Aplikacia test_tx.exe nacita vstupny subor zadany ako druhy argument
pri volani test_tx, zisti jeho velkost, ktoru ako prvu spravu
odosle druhej strane. Ta prijme spravu velkosti ocakavanych dat a pristupi
sa k odosielanu samotnych dat.Druha strana odosle spravu o uspenosti alebo
neuspenosti odosielania dat prvej strane. Podla toho sa proces opakuje
alebo ukonci. Vyuzivaju sa COM porty (cisla, je potrebne upravit
v zdrojokoych kodoch).

Program je kompilovatelny pomocou Makefile suboru.
Textove subory sluzia ako podklad pre demonstraciu aplikacie. Su dodane
.bat subory pre jednoduche spustenie programov. 