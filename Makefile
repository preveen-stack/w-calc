#
# Sample lex/yacc Makefile
# Shawn Ostermann - Mon Sep 24, 2001
# Modified by Kyle Wheeler
#
CFLAGS = -g -Wall -Werror -O0 -L/sw/lib -I/sw/include
CC = gcc
#YACC = yacc
YACC = bison -b y

LFLAGS = -lm -lreadline -ltermcap -static

PROGRAM = wcalc
CFILES = main.c calculator.c variables.c
HFILES = calculator.h variables.h


##################################################
#
# You shouldn't need to change anything else
#
##################################################

# compute the OFILES
OFILES = ${CFILES:.c=.o}

# all of the .o files that the program needs
OBJECTS = y.tab.o lex.yy.o ${OFILES}


# How to make the whole program
# (don't forget the Lex Library "-ll")
${PROGRAM} : ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} -o ${PROGRAM} -ll ${LFLAGS}
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
	${CC} -g -c y.tab.c
y.tab.h: y.tab.c
parser.h: y.tab.h
	cmp -s y.tab.h parser.h || cp y.tab.h parser.h

# 
#  Turn the scanner.l file into lex.yy.c using "lex"
# 
lex.yy.c : scanner.l parser.h ${HFILE}
	lex scanner.l
lex.yy.o: lex.yy.c
	${CC} -g -c lex.yy.c

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
