extern int varr;

#define size 250

#define TOKSIZE 128
#define SPACE " \t\r\n\a"

#define TAB_VAR 50
#define VAR_SIZE 10
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
//#include <readline/readline.h>
//#include <readline/history.h>

typedef struct var{
char *name;
char *val;
struct var *next;
}var;

extern var* vars;

//fonctions
int nb_arg(char **);
int add_vars(char *);
int cmd_interne(char **);
var* init(char *, char *);
char* get_val(char *);
int contains_name(char *);
int change_val(char *, char *);

//prototype des commandes internes
int cat(char **);
int ls(char **);
int mk_dir(char **);
int pwd();

//prototype des commandes externes
int alias(int, char **);
int unalias(int, char **);
int cd(char **);
int type(int, char **);
int history(int, char **);
int my_export(int, char **);
int my_umask(int, char **);
int my_exit(int, char **);
int my_echo(int, char **);

