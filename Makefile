# nc_2048 Makefile.

CC = clang
CFLAGS = -ggdb3 -O0 -Qunused-arguments -std=c11 -Wall -Werror
EXE = nc_2048
HDRS = nc_2048.h
LIBS = -lncurses
SRCS = display.c logic.c nc_2048.c
OBJS = $(SRCS:.c=.o)

$(EXE): $(OBJS) $(HDRS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(HDRS) Makefile

clean:
	rm -f core $(EXE) *.o

