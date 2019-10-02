#include "commandes.h"


int puissance(int base, int occ){
  int pui = 1;
  for(int i = 0; i < occ; i++){
    pui *= base;
  }
  return pui;
}


int my_umask(int argc, char **argv){  
  if(argc == 1){
    mode_t mask = umask(0);
    printf("%04o\n", mask);
    umask(mask);
  } else if(argc == 2){
    int i = 0;
    while(i < strlen(argv[1])){ //vérification du nombre octal entré
      if(isdigit(argv[1][i]) == 0){ //si le caractère n'est pas un chiffre
	if(i == 0){
	  printf("mpsh: umask: << %s >> : opérateur de mode symbolique non valable \n", argv[1]);
	} else {
	  printf("mpsh: umask: %s : nombre octal hors plage \n", argv[1]);
	}
	return 1;
      }
      if(argv[1][i] < '0' || argv[1][i] > '7'){ //si le caractère, étant un chiffre, est un chiffre compris entre 0 et 7
	printf("mpsh: umask: %s : nombre octal hors plage\n", argv[1]);
	return 1;
      }
      i++;
    }
    //args[1] n'a que des caractères entre 0 et 7
    int mask = 0;
    int p = strlen(argv[1])-1;
    for(int i = 0; i < strlen(argv[1]); i++){
      mask += (argv[1][i]-'0')*(puissance(8,p));
      p--;
    }    
    umask(mask);
  } else {
    printf("mpsh: umask: trop d'arguments ! \n");
    return 1;
  }  
  return 0;
}


