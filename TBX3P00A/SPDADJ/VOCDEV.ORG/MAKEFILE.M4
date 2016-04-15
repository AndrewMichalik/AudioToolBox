include(/usr/`include'/carl/config.m4)dnl
#################################################################
# pvdsp - phase vocoder supported by 56000-based FFTs
#################################################################
PROG =		pvdsp
CFLAGS = -O
LIBS = -lNeXT_s -lsys_s -larrayproc -ldsp_s -lm 
#################################################################
DESTDIR =	m4DESTDIR

MAIN =	main.o
DSUBS =	carith.o checkroots.o convert.o findroots.o laguerre.o \
	lpa.o lpamp.o makewindows.o oscbank.o readinput.o fft.o shiftin.o \
	shiftout.o unconvert.o
ISUBS =	fold.o overlapadd.o 

OBJS =	$(MAIN) $(DSUBS) $(ISUBS)

${PROG}: ${OBJS}
	cc ${CFLAGS} ${OBJS} ${LIBS} -o ${PROG}

$(MAIN}.o:
${DSUBS}.o: pv.h

install:	${PROG}
		cp ${PROG} ${DESTDIR}

clean:
	rm -f *.o core

NAME = test

$(NAME): $(NAME).c fft.o
	$(CC) $(CFLAGS) $(NAME).c fft.o -o $(NAME) $(LIBS)
