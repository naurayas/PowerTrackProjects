OBJ:=$(patsubst %.c,%.o,$(wildcard *.c));
client:$(OBJ)
	gcc -o $@ $^
clean:
	rm -f client *.o