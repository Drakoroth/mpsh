#include "commandes.h"


int cd(char **argv){
  if(argv[1] == NULL){
    char* ret = getenv("HOME");
    if(ret  == NULL || chdir(ret) != 0){
      printf("Impossible d'aller à ~\n");
      return 1;
    }
  } else {
    if(chdir(argv[1]) != 0){      
      printf("bash: cd: %s: Aucun fichier ou dossier de ce type \n", argv[1]);
      return 1;
    }
  }      
  return 0;
}

