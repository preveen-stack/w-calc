#
# Sample lex/yacc Makefile
# Shawn Ostermann - Mon Sep 24, 2001
# Modified by Kyle Wheeler
#
OPT=-O3
#OPT=-O0 -g
CFLAGS = -Wall -Werror ${OPT} -L/sw/lib -I/sw/include
CC = gcc
YACC = yacc
#YACC = bison -b y

LFLAGS = -ll

PROGRAM = wcalc
CFILES = main.c calculator.c variables.c string_manip.c
HFILES = calculator.h variables.h string_manip.h


##################################################
#
# You shouldn't need to change anything else
#
##################################################

# compute the OFILES
OFILES = ${CFILES:.c=.o}

# all of the .o files that the program needs
OBJECTS = y.tab.o lex.yy.o ${OFILES}

all: ${PROGRAM}

rh: ${PROGRAM}-rh

# How to make the whole program
# (don't forget the Lex Library "-ll")
${PROGRAM} : ${OBJECTS} libreadline.a
	${CC} ${OBJECTS} libreadline.a -ltermcap -o ${PROGRAM} ${LFLAGS}

${PROGRAM}-rh : ${OBJECTS}
	${CC} ${OBJECTS} -lreadline -lm -ltermcap -o ${PROGRAM} ${LFLAGS}

distro : ${PROGRAM}
	strip ${PROGRAM}

# 
# Turn the parser.y file into y.tab.c using "yacc"
# 
# Also, yacc generates a header file called "y.tab.h" which lex needs
# It's almost always the same, so we'll have lex use a different
# file and just update it when y.tab.h changes (to save compiles)
#
y.tab.c : parser.y ${HFILES}
	${YACC} -dvt ${YFLAGS} parser.y
y.tab.o: y.tab.c
	${CC} ${OPT} -c y.tab.c
y.tab.h: y.tab.c
parser.h: y.tab.h
	cmp -s y.tab.h parser.h || cp y.tab.h parser.h

# 
#  Turn the scanner.l file into lex.yy.c using "lex"
# 
lex.yy.c : scanner.l parser.h ${HFILE}
	flex scanner.l
lex.yy.o: lex.yy.c
	${CC} ${OPT} -c lex.yy.c

#
# File dependencies
#
${OFILES}: ${HFILE} parser.h

clean:
	/bin/rm -f *.o lex.yy.c y.tab.c ${PROGRAM} parser.h y.output y.tab.h core parser.tab.c

install: ${PROGRAM} ${PROGRAM:%=%.1}
	install ${PROGRAM} /usr/local/bin/${PROGRAM}
	install ${PROGRAM:%=%.1} /usr/share/man/man1/${PROGRAM:%=%.1}

uninstall:
	rm -f /usr/local/bin/${PROGRAM} /usr/share/man/man1/${PROGRAM:%=%.1}
