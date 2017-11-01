all:
	gcc client_framework.c client_op.c -o client 
	gcc server_framwork.c server_op.c -o server -lpthread -lsqlite3

clean:
	rm client server 
