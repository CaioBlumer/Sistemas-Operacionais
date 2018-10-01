all: server cliente

first: server.c
	gcc -lpthread -Wimplicit-function-declaration -o server server.c
second: cliente.c
	gcc -o client cliente.c -lpthread
