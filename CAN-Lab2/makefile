all:client server
client: client.c
	gcc -o client client.c -lm
server: server.c
	gcc server.c -lpthread -o server -lm
clean:
	rm -rf client server