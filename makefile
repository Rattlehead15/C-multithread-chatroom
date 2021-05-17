ARGS = -g -pthread

all: RemoteClient.exe RemoteMultiThreadServer.exe

RemoteClient.exe: RemoteClient.c
	gcc $(ARGS) RemoteClient.c -o RemoteClient.exe

RemoteMultiThreadServer.exe: RemoteMultiThreadServer.c
	gcc $(ARGS) RemoteMultiThreadServer.c slist.c -o RemoteMultiThreadServer.exe

clean:
	rm *.exe