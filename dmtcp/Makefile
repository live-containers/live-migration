CC=gcc
DMTCP_INCLUDE=/home/csegarra/Work/VIRT/code/dmtcp/include
CFLAGS += -I${DMTCP_INCLUDE} -DDMTCP -fPIC

all: counter

counter:
	${CC} ${CFLAGS} counter.c -o $@

clean:
	rm -rf counter
