FILE =  main.c 
EXEC = main
DEVICE = /dev/ttyUSB0

all: ${EXEC}



${EXEC}: ${FILE} Makefile
	gcc -g ${FILE} -o $@ -lm
	#lm32-elf-gcc  -static ${FILE1} -o test.o -lm
	

execute: 
	${EXEC} -l log.out -d ${DEVICE} -b 115200  -f /home/beren/git/milkymist/software/bios/bios.bin


#/home/beren/git/milkymist_linux/linux-milkymist/vmlinux.bin


#/home/beren/git/lattice-digilent_m/software/bios/bios.bin


#/media/dat_2/lattice/lm32linux-20080206/u-boot/build/u-boot.bin


	#
	#/home/beren/git/milkymist-digilent/software/bios/bios.bin
	#
	#/home/beren/git/lattice-digilent/software/bios/bios.bin
	#/media/dat_2/lattice/lm32linux-20080206/u-boot/build/u-boot.bin
	#image-2.0.x.flashbz 
	# bios.bin
	#

debug:
	gdb --args main -l log.out -d ${DEVICE} -b 115200  -f main.c

clean:
	rm -rf *.o ${EXEC} output*
