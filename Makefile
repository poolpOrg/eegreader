PROG=	eegreader

SRCS=	eegreader.c
OBJS=	$(SRCS:.c=.o)
BINDIR=		/usr/local/bin

CFLAGS+=	-fstack-protector-all
CFLAGS+=	-Wall -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=	-Wmissing-declarations
CFLAGS+=	-Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+=	-Wsign-compare
CFLAGS+=	-Werror-implicit-function-declaration
CFLAGS+=	-Werror # during development phase (breaks some archs)

@: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

clean:
	rm -f $(PROG) $(OBJS)

install: $(PROG)
	install -m 111 $(PROG) $(BINDIR)