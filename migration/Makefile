ROOT = $(shell pwd)
CC = gcc
OUT_FILE = net_utils
LD_ADD = -lssh -lpthread -lrt -lcrypto -lz -ldl
INC = /home/csegarra/Work/VIRT/libs/libssh-0.9.3/include
LIBSSH = ${ROOT}/libs/libssh-0.9.3/build/src/libssh.a

all: migration.o net_utils.o

migration.o: migration.c migration.h net_utils.h
	${CC} -I${INC} migration.c ${LIBSSH} ${LD_ADD} -lssh -o migration

net_utils.o: net_utils.c net_utils.h
	${CC} -I${INC} net_utils.c ${LIBSSH} ${LD_ADD} -lssh -o net_utils

clean:
	rm net_utils migration
