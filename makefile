all: cliente server

first: cliente.c
	gcc -o client cliente.c
second: server.c
	gcc -o server server.c
