CC = gcc
CFLAGS = -Wall -Werror -g -fsanitize=address
LDFLAGS = -lreadline

SRCS = plaidsh.c tokenize.c pipeline.c parse.c
OBJS = $(SRCS:.c=.o)
TEST_SRCS = ps_test.c tokenize.c pipeline.c parse.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

all: plaidsh ps_test

plaidsh: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

ps_test: $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f plaidsh ps_test *.o

.PHONY: all clean