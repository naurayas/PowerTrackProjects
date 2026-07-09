OBJ:=$(patsubst %.c,%.o,$(wildcard s2html_conv.c s2html_event.c s2html_main.c));
html:$(OBJ)
	gcc -o $@ $^
clean:
	rm -f html *.o