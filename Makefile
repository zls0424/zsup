CFLAGS = -g -Wall -Werror

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

all: zsup

clean:
	-rm -f zsup $(OBJS)

zsup: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
