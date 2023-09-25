all:
	gcc seagate.c recordPaddedReg.c include/list.c -o seagate.o
val:
	gcc seagate.c -g -Wall -o seagate.o
clean:
	rm seagate.o