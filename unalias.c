#include "commandes.h"


int unalias(int argc, char *argv[]){
  char chemin[size];
  getcwd(chemin, sizeof(chemin)); //copie le chemin courant
  if(argc == 1){ //unalias sans argument
    printf("unalias : utilisation : unalias nom [nom ...] \n");
  } else { //unalias avec arguments
    for(int i = 1; i < argc; i++){
      chdir(getenv("HOME")); //déplacement dans home
      FILE *fichier = NULL;
      fichier = fopen(".mpshrc", "r"); //ouvre en lecture
      char *tab_line[2];
      char *token;
      int ind = 0;
      if(fichier != NULL){ //si le fichier existe
        char ligne[size];
	int nb_line = 0;
	int compteur = 0;
	while(fgets(ligne, size, fichier) != NULL) nb_line++; //compte le nombre de lignes
	rewind(fichier); //déplacement au début du fichier
	if(nb_line > 0){
	  while(fgets(ligne, size, fichier) != NULL){ //fgets lit ligne par ligne du fichier
	    token = strtok(ligne, "=");
	    ind = 0;
	    while(token != NULL){
	      tab_line[ind++] = token; //tableau contenant la variable et la valeur de la ligne
	      token = strtok(NULL, "=");	
	    }
	    if(strcmp(argv[i], tab_line[0]) == 0){ //si l'alias existe dans le fichier
	      FILE *fichier2 = NULL;
	      fichier2 = fopen(".mpshrc2", "w+"); //lecture et écriture avec suppression du contenu au préalable
	      if(fichier2 != NULL){
		rewind(fichier); //déplacement au début du fichier		
		while(fgets(ligne, size, fichier) != NULL){	  
		  token = strtok(ligne, "=");
		  ind = 0;
		  while(token != NULL){
		    tab_line[ind++] = token; //tableau contenant la variable et la valeur de la ligne
		    token = strtok(NULL, "=");	
		  }
		  if(strcmp(argv[i], tab_line[0]) != 0){ //s'il s'agit de la ligne à remplacer 
		    fprintf(fichier2, "%s=%s", tab_line[0], tab_line[1]); //on copie les lignes de fichier à fichier2
		  }
		}
	      }
	      fclose(fichier2);
	      remove(".mpshrc");
	      rename(".mpshrc2", ".mpshrc");
	    } else {
	      compteur++;
	      if(nb_line == compteur){ //aucune ligne ne correspond à l'argument en entrée
		printf("bash: unalias: %s : non trouvé \n", argv[i]);
	      }
	    }
	  }
	} else { //aucun alias à supprimer (fichier vide)
	  printf("bash: unalias: %s : non trouvé \n", argv[i]);
	}
      } else { //fichier inexistant
	printf("bash: unalias: %s : non trouvé \n", argv[i]);
      }
      fclose(fichier);
    }
  }
  chdir(chemin);
  return 0;
}

