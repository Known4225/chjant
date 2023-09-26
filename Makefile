all:
	gcc seagate.c -c -o seagate.obj
	gcc recordPaddedReg.c -c -o recordPaddedReg.obj
	gcc include/list.c -c -o include/list.obj
	gcc seagate.obj recordPaddedReg.obj include/list.obj -o seagate.o
val:
	gcc seagate.c -g -Wall -o seagate.o
clean:
	rm seagate.o