CFLAG=

all: client server

flavor: CFLAG += -DFLAVOR
flavor: server client.o lib/readline.o
	gcc -o client $(CFLAG) -lreadline client.o lib/readline.o

client: client.o
	gcc -o client $(CFLAG) client.o

client.o: client.c
	gcc -c -o client.o $(CFLAG) client.c

server: server.o lib/recvsend.o lib/user.o
	gcc -o server $(CFLAG) server.o lib/recvsend.o lib/user.o

server.o: server.c lib/header.h
	gcc -c -o server.o $(CFLAG) server.c

lib/recvsend.o: lib/recvsend.c lib/header.h
	gcc -c -o lib/recvsend.o $(CFLAG) lib/recvsend.c

lib/user.o: lib/user.c lib/header.h
	gcc -c -o lib/user.o $(CFLAG) lib/user.c

lib/readline.o: lib/readline.c
	gcc -c -o lib/readline.o $(CFLAG) lib/readline.c

.PHONY: clean
clean:
	@rm -f client client.o server server.o lib/recvsend.o lib/user.o lib/readline.o
	@echo "Cleaned"
