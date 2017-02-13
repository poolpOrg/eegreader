PROG=	eegreader

SRCS=	eegreader.c
NOMAN=
BINDIR=		/usr/local/bin

LDADD+=
DPADD+=

CFLAGS+=	-fstack-protector-all
CFLAGS+=	-Wall -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+=	-Wmissing-declarations
CFLAGS+=	-Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+=	-Wsign-compare
CFLAGS+=	-Werror-implicit-function-declaration
CFLAGS+=	-Werror # during development phase (breaks some archs)

.include <bsd.prog.mk>
