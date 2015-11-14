#服务端makfile
server:command.o main.o
	gcc -o server command.o main.o -lpthread
command.o:command.c common.h command.h
	gcc -g -c command.c 
main.o:main.c common.h command.h
	gcc -g -c main.c -lpthread
clean:
	rm *.o
