OBJ:=$(patsubst %.c,%.o,$(wildcard *.c));
rbtree:$(OBJ)
	gcc -o $@ $^
clean:
	rm -f rbtree *.o