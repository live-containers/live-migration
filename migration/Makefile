CC = gcc
OUT_FILE = net_utils
LD_ADD = -lssh -lpthread -lrt -lcrypto -lz -ldl
INC = /home/csegarra/Work/VIRT/libs/libssh-0.9.3/include
LIBSSH = /home/csegarra/Work/VIRT/libs/libssh-0.9.3/build/src/libssh.a

main.o: net_utils.c
	${CC} -I${INC} net_utils.c ${LIBSSH} ${LD_ADD} -lssh -o ${OUT_FILE}

clean:
	rm ${OUT_FILE}
