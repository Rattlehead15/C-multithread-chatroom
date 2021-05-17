ARGS = -g -pthread

all: RemoteClient.exe servidor.exe

RemoteClient.exe: RemoteClient.c
	gcc $(ARGS) RemoteClient.c -o cliente.exe

RemoteMultiThreadServer.exe: RemoteMultiThreadServer.c
	gcc $(ARGS) RemoteMultiThreadServer.c slist.c -o RemoteMultiThreadServer.exe

clean:
	rm *.exe