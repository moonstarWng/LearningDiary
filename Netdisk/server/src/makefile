SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
CC:=gcc
server:$(OBJS)
	$(CC) $^ -o $@ -lpthread -lcrypt -ll8w8jwt `mysql_config --cflags --libs`
%.o:%.c
	$(CC) -c $^ -o $@ -g
clean:
	$(RM) $(OBJS) server
rebuild:clean all