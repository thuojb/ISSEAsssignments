CFLAGS=-Wall -Werror -g -fsanitize=address
TARGETS=et_test


all: $(TARGETS)

et_test : expr_tree.c expr_tree.h et_test.c
	gcc $(CFLAGS) $^ -lm -o $@


clean:
	rm -f $(TARGETS)
