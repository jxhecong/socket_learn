server:server.o rw_server.o list_hc.o
	gcc -o server server.o rw_server.o list_hc.o -lpthread
server.o:server.c list_hc.h rw_server.h
	gcc -c server.c -lpthread
rw_server.o:rw_server.c rw_server.h list_hc.h
	gcc -c rw_server.c -lpthread
list_hc.o:list_hc.c list_hc.h
	gcc -c list_hc.c
.PHONY:clean
clean:
	rm -f server server.o rw_server.o list_hc.o
