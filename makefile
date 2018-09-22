all: server cliente

first: server.c
	gcc -lpthread -o server server.c
second: cliente.c
	gcc -o client cliente.c -lpthread
