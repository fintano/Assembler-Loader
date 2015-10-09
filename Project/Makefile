all : main

main : main.o linkedlist.o UsefulHeap.o loader.o
	gcc -o main.out main.o linkedlist.o UsefulHeap.o loader.o -Wall

loader.o : loader.c
	gcc -c loader.c -Wall

UsefulHeap.o : UsefulHeap.c
	gcc -c UsefulHeap.c -Wall

main.o : main.c
	gcc -c main.c -Wall

linkedlist.o : linkedlist.c
	gcc -c linkedlist.c -Wall

clear :
	rm -f *.o main.out

