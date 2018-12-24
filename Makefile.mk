run: main.c
    gcc -lrt -c main.c -lpthread
    gcc main.o -lrt -o main.out -lpthread