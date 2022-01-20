all: build run 
.SILENT: build run 

build: 
	g++ -Wall -o webcam.o -c webcam.cpp
	g++ -Wall -o webcam.bin webcam.o
	gcc -Wall webcam_info.c -o helloworld

run: 
	./helloworld 10

clean: 
	rm webcam.o webcam.bin *.jpeg helloworld info.txt 

.PHONY: all
