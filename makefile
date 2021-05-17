ARGS = -g -pthread

all: cliente servidor

cliente: RemoteClient.c
	gcc $(ARGS) RemoteClient.c -o cliente

servidor: RemoteMultiThreadServer.c
	gcc $(ARGS) RemoteMultiThreadServer.c slist.c -o servidor

clean:
	rm servidor cliente