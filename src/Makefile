# Project: Implementace překladače imperativního jazyka IFJ18.
# author Radoslav Grenčík  <xgrenc00@stud.fit.vutbr.cz>
# author René Bolf         <xbolfr00@stud.fit.vutbr.cz>
# author Barbora Nemčeková <xnemce06@stud.fit.vutbr.cz>
# author Šimon Šesták      <xsesta06@stud.fit.vutbr.cz>

CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -lm -g
# LFLAGS =

SRC = scanner.c parser.c prec_table.c generator.c symstack.c symtable.c
BIN = ccifj18

ZIPFILE = ../xgrenc00

################################################################################

.PHONY: pack clean symtable symtable_vypis symstack generator

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

run: $(BIN)
	./$(BIN) < ../test/subor.src > ../test/prg.code

interpret:
	../test/ic18int ../test/prg.code < ../test/prg.in > ../test/prg.out

memtest: $(BIN)
	valgrind ./$(BIN) < ../test/subor.src #> ../test/tokens.txt

# test generator.c
generator: generator.c
	$(CC) $(CFLAGS) -DTESTING=1 generator.c -o generator
	./generator

# test symtable.c
symtable: symtable.c
	$(CC) $(CFLAGS) -DTESTING=1 symtable.c -o symtable
	./symtable

symtable_vypis: symtable.c
	$(CC) $(CFLAGS) -DTESTING=1 -DCELKOVY_VYPIS=1 symtable.c -o symtable_vypis
	./symtable_vypis

# test symstack.c
symstack: symstack.c
	$(CC) $(CFLAGS) -DTESTING=1 symstack.c -o symstack
	./symstack

# test scanner.c ale uz nebude fungovat
scanner: scanner.c
	$(CC) $(CFLAGS) -DTESTING=1 scanner.c -o scanner

test: scanner
	./scanner < ../test/subor.src > ../test/tokens.txt

example1: scanner
	./scanner < ../test/example1.src > ../test/tokens.txt

example2: scanner
	./scanner < ../test/example2.src > ../test/tokens.txt

example3: scanner
	./scanner < ../test/example3.src > ../test/tokens.txt

valgrind: scanner
	valgrind ./scanner < ../test/example2.src > ../test/tokens.txt

# zabali projekt
pack: clean
	rm -f $(ZIPFILE).zip
	mkdir -p ../zip
	cp ./* ../dokumentace.pdf ../rozdeleni ../zip
	cd ../zip; zip $(ZIPFILE).zip *
	rm -rf ../zip

# urobi poriadok
clean:
	rm -f *.o *.out $(BIN) symtable symtable_vypis symstack generator scanner ../test/tokens.txt
	touch ../test/tokens.txt

################################################################################
