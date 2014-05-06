# automatically generated makefile by ./scripts/create-make

CFLAGS=-Wall -g -O2 -std=gnu99 -I./include -L./lib

LIBS=-lpthread -litsky
LRT=-lrt

all: Server Client Server_Thread

clean:
	rm -f lib/libitsky.a  lib/fsort.o lib/fsort-metrics.o lib/isort.o lib/ternary-hsort.o lib/lib.o lib/msort.o lib/psort.o lib/hsort.o lib/sort_common.o Server Client Server_Thread

# external Libraries by Karl Brodowsky 

lib/fsort.o: lib/fsort.c
	gcc -c $(CFLAGS) lib/fsort.c -o lib/fsort.o

lib/fsort-metrics.o: lib/fsort-metrics.c
	gcc -c $(CFLAGS) lib/fsort-metrics.c -o lib/fsort-metrics.o

lib/isort.o: lib/isort.c
	gcc -c $(CFLAGS) lib/isort.c -o lib/isort.o

lib/ternary-hsort.o: lib/ternary-hsort.c
	gcc -c $(CFLAGS) lib/ternary-hsort.c -o lib/ternary-hsort.o

lib/lib.o: lib/lib.c
	gcc -c $(CFLAGS) lib/lib.c -o lib/lib.o

lib/msort.o: lib/msort.c
	gcc -c $(CFLAGS) lib/msort.c -o lib/msort.o

lib/psort.o: lib/psort.c
	gcc -c $(CFLAGS) lib/psort.c -o lib/psort.o

lib/hsort.o: lib/hsort.c
	gcc -c $(CFLAGS) lib/hsort.c -o lib/hsort.o

lib/sort_common.o: lib/sort_common.c
	gcc -c $(CFLAGS) lib/sort_common.c -o lib/sort_common.o

#own Files created by Micha Schönenberger


Server: Server.c lib/libitsky.a include/itskylib.h
	gcc $(CFLAGS) Server.c $(LIBS) -o Server    $(LRT)

Client: Client.c lib/libitsky.a include/itskylib.h
	gcc $(CFLAGS) Client.c $(LIBS) -o Client    $(LRT)
    
Client: Server_Thread.c lib/libitsky.a include/itskylib.h
	gcc $(CFLAGS) Server_Thread.c $(LIBS) -o Server_Thread    $(LRT)


lib/libitsky.a: lib/fsort.o lib/fsort-metrics.o lib/isort.o lib/ternary-hsort.o lib/lib.o lib/msort.o lib/psort.o lib/hsort.o lib/sort_common.o
	ar crs lib/libitsky.a lib/fsort.o lib/fsort-metrics.o lib/isort.o lib/ternary-hsort.o lib/lib.o lib/msort.o lib/psort.o lib/hsort.o lib/sort_common.o