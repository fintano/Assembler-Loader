all : main

main : 20101682.o linkedlist.o UsefulHeap.o loader.o
	gcc -o 20101682.out 20101682.o linkedlist.o UsefulHeap.o loader.o -Wall

loader.o : loader.c
	gcc -c loader.c -Wall

UsefulHeap.o : UsefulHeap.c
	gcc -c UsefulHeap.c -Wall

20101682.o : 20101682.c
	gcc -c 20101682.c -Wall

linkedlist.o : linkedlist.c
	gcc -c linkedlist.c -Wall

clear :
	rm -f *.o 20101682.out

