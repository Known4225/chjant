all:
	gcc chjant.c recordPaddedReg.c include/list.c -o chjant.o
val:
	gcc chjant.c recordPaddedReg.c include/list.c -g -Wall -o chjant.o
clean:
	rm chjant.o