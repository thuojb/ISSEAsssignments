# -fsanitize=... documentation:
#   https://gcc.gnu.org/onlinedocs/gcc-11.4.0/gcc/Instrumentation-Options.html
# 	https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer

CFLAGS=-Wall -Werror -g -fsanitize=address
TARGETS=clist_test


all: $(TARGETS)

clist_test : clist.c clist_test.c clist.h
	gcc $(CFLAGS) $^ -o $@


clean:
	rm -f $(TARGETS)

