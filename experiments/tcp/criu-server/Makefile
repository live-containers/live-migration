CC = gcc
OBJ = echo

all: server client add_rule

server: server.c
	${CC} -Wall server.c -o server

client: client.c
	${CC} -Wall client.c -o client

add_rule: add_rule.c
	${CC} -Wall add_rule.c -o add_rule

clean:
	rm server add_rule client
