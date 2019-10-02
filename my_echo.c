#include "commandes.h"


int my_echo(int argc, char **argv){
  if(argc <= 1) printf("\n"); //pas d'argument
  else {
    for(int i = 1; i < argc; i++){
      if(argv[i][0] == '$'){ //contenu d'une variable
	char *token;
	token = strtok(argv[i], "$");
	if(getenv(token) != NULL){
	  printf("%s ", getenv(token));
	} else {
	  if(contains_name(token) == 0){
	    char *val = get_val(token);
	    printf("%s\n", val);
	  }
	}
      } else if(argv[i][0] == '\\'){
	char *token;
	token = strtok(argv[i], "\\");
        printf("%s ", token);
      } else {
	if(argv[1][0] == '"'){
	  char *token;
	  token = strtok(argv[1], "\"");
	  printf("%s", token);
	} else {
	  printf("%s ", argv[i]);
	}
      }
    }
    printf("\n");
  }
  return 0;
}


/* suite code variables locales
int nb = nb_arg(vars);
for(int i = 0; i < nb; i++){
  if(i%2 == 0){
    if(strcmp(token, vars[i]) == 0){
      printf("%s \n", vars[i+1]);
    }
  }
}
*/
