CC= /usr/bin/gcc
all:	client tcpserver udpserver runServer runClient

endScript:
	exit

client: client.c;
	${CC} client.c -o client

tcpserver: tcpserver.c;
	${CC} tcpserver.c -o tcpserver

udpserver: udpserver.c;
	${CC} udpserver.c -o udpserver

clean:
	rm client tcpserver udpserver

kill:
	pgrep -f "./*server" | xargs kill -9

runServer: 
	./tcpserver&
	./udpserver&

runClient:
	./client
	
.PHONY: do_script

do_script: 
	script runningclient.txt

prerequisites: do_script

target: prerequisites