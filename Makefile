all:	compiler server client
compiler:	lex.yy.c y.tab.c
	gcc -o compile lex.yy.c y.tab.c
lex.yy.c:	mid.l
	lex mid.l
y.tab.c:	mid.y
	yacc -d mid.y

server:	server.c
	gcc -o server server.c
client:	client.c
	gcc -o client client.c

clean:
	rm server client lex.yy.c y.tab.c y.tab.h compile pid_*.txt script_*.sh file_*.c 
