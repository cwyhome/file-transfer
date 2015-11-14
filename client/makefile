#客户端makefile
client:main.o command.o input.o
	gcc -o client command.o input.o main.o
command.o:command.h command.c
	gcc -g -c command.c
input.o:command.h input.h input.c
	gcc -g -c input.c
main.o:command.h input.h main.c
	gcc -g -c main.c
clean:
	rm *.o
