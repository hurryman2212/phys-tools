SRCS = $(wildcard *.c)

PROGS = $(patsubst %.c,%,$(SRCS))

CFLAGS += -std=gnu11 -D_GNU_SOURCE -Wall -Wextra -Werror -pedantic -Wno-unused-function

all: $(PROGS)
%: %.c
	$(CC) $(CFLAGS) -o $@ $< -lx86linuxextra -lbacktrace

install: all
	cp $(PROGS) /usr/local/bin/
uninstall:
	rm -f $(PROGS:%=/usr/local/bin/%)

clean:
	rm -f $(PROGS)
