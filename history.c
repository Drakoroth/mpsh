#include "commandes.h"


int history(int arg, char **args){
  char *c = malloc(size);
  getcwd(c, size); //copie le chemin absolu du répertoire dans c
  if(arg == 1){ //history sans argument
    chdir(getenv("HOME")); //emplacement du fichier "bash_history"
    FILE* fichier = NULL;
    fichier = fopen(".bash_history", "r"); //ouvrir le fichier en lecture
    if(fichier != NULL){
      char ligne[size];
      int num = 1;
      while(fgets(ligne, size, fichier) != NULL){ //fgets lit ligne par ligne du fichier
	if(num >= 1 && num <= 9) printf("    %d  %s", num, ligne);
	if(num >= 10 && num <= 99) printf("   %d  %s", num, ligne);
	if(num >= 100 && num <= 999) printf("  %d  %s", num, ligne);
	if(num >= 1000 && num <= 2000) printf(" %d  %s", num, ligne);
	num++;
      }
    }
    fclose(fichier);
  }
  else if(arg == 2){ //history avec argument
    if(isdigit(args[1][0]) != 0){ //le premier char est un nb alors on continue la vérification
      int verif = 0;
      for(int i = 0; i < strlen(args[1]); i++){
	if(isdigit(args[1][i]) != 0){
	  verif++;
	}
      }
      if(verif == strlen(args[1])){ //si l'argument entier est bien un nombre alors on continue
	int nb = atoi(args[1]); //nb = l'argument n correspondant à la ligne que l'on veut exécuter
	chdir(getenv("HOME")); //emplacement du fichier "bash_history"
	FILE* fichier = NULL;
	fichier = fopen(".bash_history", "r"); //ouvrir le fichier en lecture
	if(fichier != NULL){
	  char ligne[size];
	  int compteur = 0;
	  while(fgets(ligne, size, fichier) != NULL){ //compter le nb de lignes du fichier "bash_history"
	    compteur++;
	  }
	  if(nb <= compteur){ //si l'argument n est plus petit que le nb de lignes du fichier
	    fseek(fichier, 0, SEEK_SET); //replace le curseur au début du fichier
	    compteur = nb;
	    while((fgets(ligne, size, fichier) != NULL) && (compteur-3 > 0)){ //on se place au bon endroit
	      compteur--;
	    }
	    verif = 0;
	    char *tab; //init chaine de caractère
	    while(fgets(ligne, size, fichier) != NULL && verif == 0){ 
	      tab = ligne; //chaine de caractères contenant la ligne de la commande correspondante
	      verif = 1;
	    }
	    if(tab[strlen(tab)-1] == '\n'){ //supprimer le saut de ligne
	      tab[strlen(tab)-1] = '\0';
	    }
	    int esp = 0;
	    for(int i = 0; i < strlen(tab); i++){
	      if(tab[i] == ' ') esp++;
	    }
	    char *ch[esp+2]; //init du tableau qui contiendra la chaine de caractère
	    char *token = strtok(tab, " ");
	    int j = 0;
	    while(token != NULL){
	      ch[j] = token; //on place chaque élément de la ligne de commande dans le tableau
	      j++;
	      token = strtok(NULL, " ");
	    }
	    ch[esp+1] = NULL; //dernière case du tableau doit être NULL
	    pid_t pid;
	    int status;
	    if((pid = fork()) < 0){
	      fprintf(stderr, "fork error: %s\n", strerror(errno));
	      return EXIT_FAILURE;
	    }
	    if(pid == 0){
	      if(execvp(ch[0], ch) < 0){
		fprintf(stderr, "mpsh: commande introuvable: %s\n", args[0]);
		return EXIT_FAILURE;
	      }
	      exit(EXIT_FAILURE);
	    } else {
	      do {
		waitpid(pid, &status, WUNTRACED);
	      } while(!WIFEXITED(status) && !WIFSIGNALED(status));
	    }
	    chdir(c);
	    return 0;
	  }
	  else {
	    printf("bash: history: %s : argument numérique trop grand \n", args[1]);
	    return 1;
	  }
	}
      }
      else {
	printf("bash: history: %s : argument numérique nécessaire \n", args[1]);
	return 1;
      }
    }
    else if(args[1][0] == '-'){ //le premier char est un tiret alors on vérifie que la suite est un nb
      int verif = 0;
      for(int i = 1; i < strlen(args[1]); i++){
	if(isdigit(args[1][i]) != 0){
	  verif++;
	}
      }
      if(verif == (strlen(args[1])-1)){ //si l'argument suivant le tiret est bien un nombre alors on continue
	int mult = 1;
	int tmp = strlen(args[1]) - 2;
	while(tmp > 0){
	  mult *= 10;
	  tmp--;
	}
	int nb = 1;
	for(int i = 1; i < strlen(args[1]); i++){
	  nb += (args[1][i] - '0') * mult; //conversion ascii char en int
	  mult /= 10;
	}
	nb--; //nb correspond au nb après le tiret qu'on récupère sous forme de int	
	chdir(getenv("HOME")); //emplacement du fichier "bash_history"
	FILE* fichier = NULL;
	fichier = fopen(".bash_history", "r"); //ouvrir le fichier en lecture
	if(fichier != NULL){
	  char ligne[size];
	  int compteur = 0;
	  while(fgets(ligne, size, fichier) != NULL){ //compter le nb de lignes du fichier "bash_history"
	    compteur++;
	  }
	  if(nb < compteur){ //si l'argument n est plus petit que le nb de lignes du fichier
	    int dif = compteur - nb - 1; //nombre de de lignes passées avant d'afficher ce qu'on veut
	    fseek(fichier, 0, SEEK_SET); //replace le curseur au début du fichier
	    while(fgets(ligne, size, fichier) != NULL && dif > 0){ //on se place au bon endroit
	      dif--;
	    }
	    dif = compteur - nb + 1; //pour afficher la numérotation à côté des commandes
	    while(fgets(ligne, size, fichier) != NULL){ 
	      if(dif >= 1 && dif <= 9) printf("    %d  %s", dif, ligne);
	      if(dif >= 10 && dif <= 99) printf("   %d  %s", dif, ligne);
	      if(dif >= 100 && dif <= 999) printf("  %d  %s", dif, ligne);
	      if(dif >= 1000 && dif <= 2000) printf(" %d  %s", dif, ligne);
	      dif++;
	    }
	  }
	  else { //si l'argument est plus grand que l'historique, on affiche tout
	    int num = 1;
	    fseek(fichier, 0, SEEK_SET); //replace le curseur au début du fichier
	    while(fgets(ligne, size, fichier) != NULL){ //fgets lit ligne par ligne
	      if(num >= 1 && num <= 9) printf("    %d  %s", num, ligne);
	      if(num >= 10 && num <= 99) printf("   %d  %s", num, ligne);
	      if(num >= 100 && num <= 999) printf("  %d  %s", num, ligne);
	      if(num >= 1000 && num <= 2000) printf(" %d  %s", num, ligne);
	      num++;
	    }
	  }
	}
	fclose(fichier);
      }
      else {
	printf("bash: history: %s : argument numérique nécessaire \n", args[1]);
	return 1;
      }
    }
    else {
      printf("bash: history: %s : argument numérique nécessaire \n", args[1]);
      	return 1;
    }
  }
  else {
    printf("bash: history: trop d'arguments \n");
    return 1;
  }
  chdir(c);
  return 0;
}
