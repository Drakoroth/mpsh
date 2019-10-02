#include "commandes.h"


int type(int arg, char **args){
  if(arg > 1){
    for(int i = 1; i < arg; i++){
      if(strcmp(args[i], "export") == 0 && args[i+1] != NULL && strcmp(args[i+1], "var") == 0){
	printf("export var est une primitive du shell \n");
	i++;
      }
      else if(strcmp(args[i], "alias") == 0 || strcmp(args[i], "cd") == 0 || strcmp(args[i], "echo") == 0 || strcmp(args[i], "exit") == 0 || strcmp(args[i], "export") == 0 || strcmp(args[i], "history") == 0 || strcmp(args[i], "type") == 0 || strcmp(args[i], "pwd") == 0 || strcmp(args[i], "umask") == 0 || strcmp(args[i], "unalias") == 0){
	printf("%s est une primitive du shell \n", args[i]);
      }
      else if(strcmp(args[i], "cat") == 0 || strcmp(args[i], "mkdir") == 0){
	printf("%s est /bin/%s \n", args[i], args[i]);
      }
      else if(strcmp(args[i], "ls") == 0){
	printf("ls est un alias \n");
      }
      else {
	printf("bash: type: %s : non trouvé \n", args[i]);
      }
    }
  }
  return 0;
}

