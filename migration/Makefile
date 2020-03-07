CC=gcc
OUT_FILE=net_utils

main.o: net_utils.c
	${CC} -c net_utils.c -lssh -o ${OUT_FILE}

clean:
	rm ${OUT_FILE}
