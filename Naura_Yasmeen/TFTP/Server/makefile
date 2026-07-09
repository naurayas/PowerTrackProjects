OBJ:=$(patsubst %.c,%.o,$(wildcard *.c));
server:$(OBJ)
	gcc -o $@ $^
clean:
	rm -f server *.o