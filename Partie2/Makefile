
tsock: message.o socket.o bal.o tsock.o
	gcc -g -Wall tsock.o message.o socket.o bal.o -o tsock

tsock.o: tsock.c
	gcc -g -Wall -c tsock.c

message.o: message.c
	gcc -g -Wall -c message.c

bal.o: bal.c
	gcc -g -Wall -c bal.c

socket.o: socket.c
	gcc -g -Wall -c socket.c
