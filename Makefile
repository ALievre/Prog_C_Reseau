
tsock_v1: message.o tcp.o udp.o tsock_v1.o
	gcc -g -Wall tsock_v1.o message.o -o tsock_v1

tsock_v1.o: tsock_v1.c
	gcc -g -Wall -c tsock_v1.c

message.o: message.c
	gcc -g -Wall -c message.c

