CC = gcc
CFLAGS = -Wall
LIBSSH = ../lib/libssh-0.9.3/build/src/libssh.a
LD_ADD = -lssh -lpthread -lrt -lcrypto -lz -ldl -lm #${LIBSSH}
BENCH = -DBENCHMARK=1
#INC =-I. -I../lib/libssh-0.9.3/include

all: migration

migration: migration.o net_utils.o time_utils.o
	${CC} ${CFLAGS} ${INC} -o migration migration.o net_utils.o time_utils.o ${LD_ADD}

migration.o: migration.c net_utils.h time_utils.h
	${CC} ${BENCH} ${CFLAGS} -c migration.c 

net_utils.o: net_utils.c
	${CC} ${CFLAGS} -c net_utils.c 

time_utils.o: time_utils.c
	${CC} ${CFLAGS} -c time_utils.c 

clean:
	rm migration *.o
