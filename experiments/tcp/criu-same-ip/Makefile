CC = gcc
OBJ = echo

all: echo client

echo: echo.c
	${CC} -Wall echo.c -o echo

client: client.c
	${CC} -Wall client.c -o client

clean:
	rm echo client
