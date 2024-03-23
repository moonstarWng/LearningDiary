SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
CC:=gcc
CFLAGS:=-g -Wno-deprecated-declarations
LDFLAGS:=`mysql_config --cflags --libs`

client: $(OBJS)
	$(CC) $^ -o $@ -lpthread -lcrypt `mysql_config --cflags --libs`
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
clean:
	$(RM) $(OBJS) client
rebuild: clean client
