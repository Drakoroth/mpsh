#include "commandes.h"


int alias(int argc, char *argv[]){
  char chemin[size];
  getcwd(chemin, sizeof(chemin)); //copie le chemin courant
  if(argc == 1){ //alias sans argument (affichage)
    chdir(getenv("HOME")); //déplacement dans home
    FILE *fichier = NULL;
    fichier = fopen(".mpshrc", "a+"); //lecture du fichier (créé si inexistant)
    if(fichier != NULL){
      char ligne[size];
      while(fgets(ligne, size, fichier) != NULL){ //fgets lit ligne par ligne du fichier
	printf("%s", ligne); //affiche chaque alias présent dans .mpshrc	
      }
    }
    fclose(fichier);
  } else { //alias avec arguments
    for(int i = 1; i < argc; i++){
      if((strstr(argv[i], "=") != NULL)){ //argument contenant le signe égal
	chdir(getenv("HOME")); //déplacement dans home
	FILE *fichier = NULL;
	fichier = fopen(".mpshrc", "a+"); //ajout (écriture) en fin de fichier (créé si inexistant)
	char *tab_arg[2];
	char *tab_line[2];
	char *token = strtok(argv[i], "=");
	int ind = 0;
        while(token != NULL) {
          tab_arg[ind++] = token; //tableau contenant la variable et la valeur de l'argument
          token = strtok(NULL, "=");
        }
	tab_arg[1] = strtok(tab_arg[1], "\"");
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
	      while(token != NULL) {
		tab_line[ind++] = token; //tableau contenant la variable et la valeur de la ligne
		token = strtok(NULL, "=");	
	      }
	      if(strcmp(tab_arg[0], tab_line[0]) == 0){ //si l'alias existe déjà dans le fichier
		FILE *fichier2 = NULL;
		fichier2 = fopen(".mpshrc2", "w+"); //lecture et écriture avec suppression du contenu au préalable
		if(fichier2 != NULL){
		  rewind(fichier); //déplacement au début du fichier
		  char old[strlen(tab_line[0])+strlen(tab_line[1])+1]; //correspond à la ligne à remplacer
		  strcpy(old, tab_line[0]);
		  strcat(old, "=");
		  strcat(old, tab_line[1]);
		  char new[strlen(tab_arg[0])+strlen(tab_arg[1])+1]; //correpond à la ligne remplaçante 
		  strcpy(new, tab_arg[0]);
		  strcat(new, "=");
		  strcat(new, tab_arg[1]);
		  strcat(new, "\n");
		  while(fgets(ligne, size, fichier) != NULL){
		    if(strcmp(ligne, old) == 0){ //s'il s'agit de la ligne à remplacer 
		      memset(ligne, '\0', sizeof(ligne)); //on vide la ligne
		      strcpy(ligne, new); //on lui affecte sa nouvelle valeur
		    }
		    fprintf(fichier2, "%s", ligne); //on copie les lignes de fichier à fichier2
		  }
		}
		fclose(fichier2);
		remove(".mpshrc");
		rename(".mpshrc2", ".mpshrc");
	      } else { //alias n'existant pas dans le fichier
		compteur++;
		if(nb_line == compteur){ //aucune ligne ne correspond à l'alias, on peut l'ajouter
		  fprintf(fichier, "%s=%s \n", tab_arg[0], tab_arg[1]); //on ajoute l'alias dans le fichier
		}
	      }
	    }
	  } else { //aucune ligne dans le fichier
	    fprintf(fichier, "%s=%s \n", tab_arg[0], tab_arg[1]); //alias à ajouter d'office car il s'agit du premier
	    return 1;
	  }
	} else {
	  printf("mpsh: alias: %s : non trouvé \n", argv[i]);
	  return 1;
	}
	fclose(fichier);
      } else { //argument ne contenant pas le signe égal
	chdir(getenv("HOME")); //déplacement dans home
	FILE *fichier = NULL;
	fichier = fopen(".mpshrc", "r"); //ouvre en lecture
	char *tab_line[2];
	char *token;
	int ind = 0;
	if(fichier != NULL){ //si le fichier existe
	  char ligne[size];
	  int verif = 0; //vérification de la variable dans le fichier	  
	  while(fgets(ligne, size, fichier) != NULL){ //fgets lit ligne par ligne du fichier	    
	    token = strtok(ligne, "=");
	    ind = 0;	    
	    while(token != NULL) {
	      tab_line[ind++] = token; //tableau contenant la variable et la valeur de la ligne
	      token = strtok(NULL, "=");	
	    }
	    if(strcmp(argv[i], tab_line[0]) == 0){ //si l'alias existe déjà dans le fichier
	      verif = 1;
	      printf("%s=%s", tab_line[0], tab_line[1]);
	    }
	  }
	  if(verif == 0){
	    printf("bash: alias: %s : non trouvé \n", argv[i]);
	    return 1;
	  }
	} else {
	  printf("bash: alias: %s : non trouvé \n", argv[i]);
	  return 1;
	}
	fclose(fichier);
      }
    }
  }
  chdir(chemin);
  return 0;
}

