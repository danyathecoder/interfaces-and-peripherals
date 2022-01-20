all: main run clean
.SILENT: main run clean

main: weird.cpp magiclib.c
	gcc magiclib.c -o magic -lpci
	g++ weird.cpp -o f_weird

run: weird
	./f_weird

clean: 
	rm f_weird magic
	
.PHONY: all
