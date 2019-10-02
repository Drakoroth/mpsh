CC=gcc
CFLAGS= -Wall -g -lreadline
ALL = mpsh

all: $(ALL)
mpsh : mpsh.o alias.o cd.o history.o my_echo.o my_export.o my_umask.o type.o unalias.o

mpsh.o : mpsh.c commandes.h
alias.o : alias.c commandes.h
cd.o : cd.c commandes.h
history.o : history.c commandes.h
my_echo.o : my_echo.c commandes.h
my_export.o : my_export.c commandes.h
my_umask.o : my_umask.c commandes.h
type.o : type.c commandes.h
unalias.o : unalias.c commandes.h

clean:
	rm -rf *.o $(ALL)
