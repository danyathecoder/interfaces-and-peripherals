all: main run clean
.SILENT: main run clean

main: normal.c
	gcc normal.c -o pci_info -lpci

run: pci_info
	./pci_info

clean: 
	rm pci_info
	
.PHONY: all
