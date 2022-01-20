all: main run clean
.SILENT: main run clean

main: powerinfo.c
	gcc powerinfo.c -o powerinfo

run: powerinfo
	./powerinfo

clean: 
	rm powerinfo

.PHONY: all
