
tsock_v4: message.o socket.o udp.o tcp.o tsock_v4.o
	gcc -g -Wall tsock_v4.o message.o socket.o udp.o tcp.o -o tsock_v4

tsock_v4.o: tsock_v4.c
	gcc -g -Wall -c tsock_v4.c

message.o: message.c
	gcc -g -Wall -c message.c

udp.o: udp.c
	gcc -g -Wall -c udp.c

tcp.o: tcp.c
	gcc -g -Wall -c tcp.c

socket.o: socket.c
	gcc -g -Wall -c socket.c
