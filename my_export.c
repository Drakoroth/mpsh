#include "commandes.h"


int my_export(int argc, char **argv){
  if(argc >= 2){
    char *tokens[2];
    char *token;
    for(int i = 1; i < argc; i++){
      int pos = 0;
      token = strtok(argv[i], "=");
      while(token != NULL){
	tokens[pos++] = token;
	token = strtok(NULL, "=");
      }
      if(contains_name(tokens[0]) == 0){
	setenv(tokens[0], get_val(tokens[0]), 1);
      } else {
	setenv(tokens[0], tokens[1], 1);
      }
    }
  } else {
    char **env = __environ;
    while(*env){
      printf("%s\n", *env);
      env++;
    }
  }
  return 0;
}
