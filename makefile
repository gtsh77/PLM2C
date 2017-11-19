# Makefile for Unix

SRCS =	convert.c	\
	parse.c		\
	declare.c	\
	decl_out.c	\
	control.c	\
	io.c		\
	token.c		\
	context.c	\
	mem.c		\
	error.c		\
	version.c	\
	main.c

OBJS =	convert.o	\
	parse.o		\
	declare.o	\
	decl_out.o	\
	control.o	\
	io.o		\
	token.o		\
	context.o	\
	mem.o		\
	error.o		\
	version.o	\
	main.o

LNKS =	convert parse declare decl_out control io token context mem error version main

TOKEN_HDRS = misc.h defs.h struct.h cvt.h cvt_id.h tokens.h
HDRS =	$(TOKEN_HDRS) tkn_defs.h tkn_ext.h

OPTS = -c -O

plm2c:	$(OBJS)
	$(CC) -o plm2c $(OBJS)

convert.o:	convert.c $(TOKEN_HDRS)
	$(CC) $(OPTS) convert.c

parse.o:	parse.c $(TOKEN_HDRS) cvt_id.h
	$(CC) $(OPTS) parse.c

declare.o:	declare.c $(TOKEN_HDRS)
	$(CC) $(OPTS) declare.c

control.o:	control.c $(TOKEN_HDRS) tkn_ext.h
	$(CC) $(OPTS) control.c

decl_out.o:	decl_out.c $(TOKEN_HDRS)
	$(CC) $(OPTS) decl_out.c

io.o:	io.c $(TOKEN_HDRS) tkn_ext.h
	$(CC) $(OPTS) io.c

token.o:	token.c $(TOKEN_HDRS) tkn_ext.h
	$(CC) $(OPTS) token.c

context.o:	context.c $(TOKEN_HDRS)
	$(CC) $(OPTS) context.c

mem.o:	mem.c $(TOKEN_HDRS)
	$(CC) $(OPTS) mem.c

error.o:	error.c $(TOKEN_HDRS)
	$(CC) $(OPTS) error.c

version.o:	version.c
	$(CC) $(OPTS) version.c

main.o:	main.c $(TOKEN_HDRS) tkn_defs.h
	$(CC) $(OPTS) main.c

backup:
	cp $(HDRS) Makefile bak
	cp $(SRCS) bak

lint:
	lint $(SRCS)

clean:
	rm -f $(OBJS)
