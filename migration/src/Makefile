CC = gcc
CFLAGS = -Wall
LIBSSH = ../lib/libssh-0.9.3/build/src/libssh.a
LD_ADD = -lssh -lpthread -lrt -lcrypto -lz -ldl -lm #${LIBSSH}
#INC =-I. -I../lib/libssh-0.9.3/include

migration: migration.o net_utils.o
	${CC} ${CFLAGS} ${INC} -o migration migration.o net_utils.o ${LD_ADD}

migration.o: migration.c net_utils.h
	${CC} ${CFLAGS} -c migration.c 

net_utils.o: net_utils.c
	${CC} ${CFLAGS} -c net_utils.c 

clean:
	rm migration *.o
