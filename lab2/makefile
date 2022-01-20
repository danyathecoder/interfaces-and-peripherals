all: main run clean
.SILENT: main run clean

main: diskinfo.c
	gcc diskinfo.c -o diskinfo

run: diskinfo
	./diskinfo

clean: 
	rm diskinfo

.PHONY: all
