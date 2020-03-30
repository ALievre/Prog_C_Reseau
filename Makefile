
tsock_v2: message.o socket.o udp.o tcp.o tsock_v2.o
	gcc -g -Wall tsock_v2.o message.o socket.o udp.o tcp.o -o tsock_v2

tsock_v2.o: tsock_v2.c
	gcc -g -Wall -c tsock_v2.c

message.o: message.c
	gcc -g -Wall -c message.c

udp.o: udp.c
	gcc -g -Wall -c udp.c

tcp.o: tcp.c
	gcc -g -Wall -c tcp.c

socket.o: socket.c
	gcc -g -Wall -c socket.c
