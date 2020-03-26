
tsock_v2: message.o tcp.o udp.o tsock_v2.o
	gcc -g -Wall tsock_v2.o message.o -o tsock_v2

tsock_v2.o: tsock_v2.c
	gcc -g -Wall -c tsock_v2.c

message.o: message.c
	gcc -g -Wall -c message.c

