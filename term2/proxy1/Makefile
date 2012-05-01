proxy1 : proxy.o http.o cache.o fdlist.o
	cc -lpthread -lsocket -lrt -lnsl -o proxy1 proxy.o http.o cache.o fdlist.o
proxy.o : proxy.c http.c cache.c fdlist.c
	cc -c proxy.c
clean :
	rm proxy.o http.o cache.o fdlist.o
