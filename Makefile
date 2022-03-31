all: serverApp

INCS = -I./

serverApp: cmdProcessing.o errexit.o icdHandler.o queue.o recvCmdFromCtrlPc.o sendResponseToCtrlPc.o server.o sock.o
	gcc -o $@ $^ -lpthread

%.o: %.c
	gcc -c -g -o $@ $<

clean:
	rm -f *.o serverApp tags
