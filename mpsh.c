#include "commandes.h"
int varr;


//affiche le répertoire courant
char *prompt;


//variables locales
var* vars;


//récupère l'entrée sur le terminal
char* read_line(){
  char *line = NULL;
  size_t len = 0;
  getline(&line, &len, stdin); 
  return line;
}


//renvoie un tableau avec chaque élément de la ligne
char **split_line(char *line){
  char **tokens;
  char *token;
  int pos = 0;
  if((tokens = malloc(sizeof(char*) * TOKSIZE)) == NULL){
    exit(EXIT_FAILURE);
  }
  if(strstr(line, "\"") == NULL){
    token = strtok(line, SPACE);
    while(token != NULL){
      tokens[pos++] = token;
      token = strtok(NULL, SPACE);
    }
  } else {
    token = strtok(line, SPACE);
     while(token != NULL){
      tokens[pos++] = token;
      token = strtok(NULL, "\t");
    }
  }
  tokens[pos] = NULL;
  return tokens;
}


//compteur pour récupérer le nombre d'arguments d'une ligne
int nb_arg(char **argv){
  int i = 0;
  while(argv[i] != NULL) i++;
  return i;
}


//commande exit
int my_exit(int argc, char *argv[]){ //récupère la valeur de retour avec $?
  if(argc == 1){ //sort du shell avec la valeur de retour de la dernière commande 
    exit(varr);   
  } else if(argc == 2){
    if(isdigit(argv[1][0]) != 0){ //le premier char est un nb alors on continue la vérification
      int verif = 0;
      for(int i = 0; i < strlen(argv[1]); i++){
	if(isdigit(argv[1][i]) != 0){
	  verif++;
	}
      } 
      if(verif == strlen(argv[1])){ //si l'argument entier est bien un nombre
	int nb = atoi(argv[1]); //conversion
	exit(nb); //sort du programme avec la valeur de retour spécifiée
      } else {
	exit(EXIT_FAILURE); //n'est pas un nombre, erreur (renvoie 1)
      }
    } else {
      exit(EXIT_FAILURE); //n'est pas un nombre, erreur (renvoie 1)
    }
  } else { //plus d'un argument -> erreur (renvoie 1)
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS); //équivalent à 0
}


//commandes internes: existantes dans /bin
int cmd_interne(char **argv){
  pid_t pid;
  int status;
  if((pid = fork()) < 0){
    fprintf(stderr, "fork error: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  if(pid == 0){
    if(execvp(argv[0], argv) < 0){
      if(argv[0][0] == '$'){
	char *token = strtok(argv[0], "$");
	if(contains_name(token) == 0){
	  fprintf(stderr, "%s: commande introuvable\n", get_val(token));
	} else {
	  return EXIT_FAILURE;
	}
      } else {
	fprintf(stderr, "mpsh: commande introuvable: %s\n", argv[0]);
      }
      return EXIT_FAILURE;
    }
    exit(EXIT_FAILURE);
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 0;
}


int cmd_exec_aux(char **argv){
  if(argv[0] == NULL) {
    return 1;
  }
  if(strcmp(argv[0], "cd") == 0){
    return cd(argv);
  }
  if(strcmp(argv[0], "echo") == 0){
    return my_echo(nb_arg(argv), argv);
  }
  if(strcmp(argv[0], "history") == 0){
    return history(nb_arg(argv), argv);
  }
  if(strcmp(argv[0], "type") == 0){
    return type(nb_arg(argv), argv);
  }
  if(strcmp(argv[0], "export") == 0){
    return my_export(nb_arg(argv), argv);
  }
  if(strcmp(argv[0], "alias") == 0){
    return alias(nb_arg(argv), argv);
  }
  if(strcmp(argv[0], "unalias") == 0){
    return unalias(nb_arg(argv), argv);
  }
  if(strcmp(argv[0], "umask") == 0){
    return my_umask(nb_arg(argv), argv);
  }
  if(strcmp(argv[0], "exit") == 0){
    return my_exit(nb_arg(argv), argv);
  }
  int res;
  for(int i = 0; i < nb_arg(argv); i++){
    if(strstr(argv[i], "=") != NULL){
      res = add_vars(argv[i]);
    } else {
      res = cmd_interne(argv);
    }
  }
  return res;
}


//pour l'exécution des alias
int cmd_alias(int argc, char **argv){
  if(argc == 0) return 0;
  char *chemin = malloc(size);
  getcwd(chemin, size); //copie le chemin absolu du répertoire dans tab
  chdir(getenv("HOME")); //déplacement dans home
  FILE *fichier = NULL;
  fichier = fopen(".mpshrc", "r"); //ouvre en lecture
  char *tab_line[2];
  char *token;
  int ind = 0;
  if(fichier != NULL){ //si le fichier existe
    char ligne[size];
    while(fgets(ligne, size, fichier) != NULL){
      token = strtok(ligne, "=");
      while(token != NULL){
	tab_line[ind++] = token;
	token = strtok(NULL, "=");
      }   
      if(strcmp(tab_line[0], argv[0]) == 0){ //si ça correspond à un alias (exécute)
	if(tab_line[1][strlen(tab_line[1])-1] == '\n'){ //supprimer le saut de ligne
	  if(argc == 1){
	    tab_line[1][strlen(tab_line[1])-1] = '\0';
	      
	  } else { //s'il y des options, on met un espace
	    tab_line[1][strlen(tab_line[1])-1] = ' ';
	    for(int i = 1; i < argc; i++){ //on concatène l'alias avec les options
	      strcat(tab_line[1], argv[i]);
	    }
	  }
	}
	char **arg = split_line(tab_line[1]);
	chdir(chemin);
	free(chemin);
	return cmd_exec_aux(arg); //exécution de la commande en alias*/
      } else ind = 0;
    }   
  }
  //s'il n'y a pas d'alias 
  chdir(chemin);
  free(chemin);
  return cmd_exec_aux(argv);
}


//supprimer les espaces avant et après pour une chaîne de caractères
void trim(char *s){
  char *ch = s;
  char *new = s;   
  while(isspace(*ch)) ++ch;  
  while(*ch){
    if(!isspace(*ch)) new = s + 1;
    *s++ = *ch++;
  }
  *new = '\0';
}


//les connecteurs logiques && et ||
void connecteurs(char *line){
  char l[strlen(line)];
  strcpy(l, line); //copie de la ligne telle quelle avant toute modification avec strtok
  char *connecteurs = "&&||"; //séparateurs
  char **toks = malloc(sizeof(char*) * TOKSIZE);
  char *tok;
  int ind = 0;
  tok = strtok(line, connecteurs);
  while(tok != NULL){
    if(ind == 0) toks[ind] = tok;
    else {
      if(isspace(tok[0])){
	for(int i = 1; i < strlen(tok); i++) tok[i-1] = tok[i];
	tok[strlen(tok)-1] = '\0';
	toks[ind] = tok;
      }
    }
    toks[ind+1] = "tmp";
    ind += 2;
    tok = strtok(NULL, connecteurs);
  }
  int a = 0;
  for(int i = 0; i < strlen(l)-1; i++){ //remplacer les tmp par les connecteurs
    if(l[i] == '&' && l[i+1] == '&'){
      toks[1+a] = "&&"; 
      a += 2;
    }
    if(l[i] == '|' && l[i+1] == '|'){
      toks[1+a] = "||";
      a += 2;
    }
  }
  if(strcmp(toks[ind-1], "tmp") == 0){ //pas de pb dans la syntaxe
    int verif = 1; //vérification pour savoir si on exécute la commande suivante
    for(int i = 0; i < ind-1; i++){
      if(i == 0){ //première case (exécution d'office)
	char **s = split_line(toks[0]);
	if(cmd_alias(nb_arg(s), s) == 0) verif = 0; //bonne exécution
	else verif = 1; //mauvaise exécution
      } else { //autres cases
	if(strcmp(toks[i], "&&") == 0 && (verif == 0)){ //exécute la commande suivante que si la première a réussi
	  char **s = split_line(toks[i+1]);
	  if(cmd_alias(nb_arg(s), s) == 0) verif = 0; 
	  else verif = 1;
	}
	if((strcmp(toks[i], "||") == 0) && (verif == 1)){ //exécute la commande suivante si la première a échoué
	  char **s = split_line(toks[i+1]);
	  if(cmd_alias(nb_arg(s), s) == 0) verif = 0;
	  else verif = 1; 
	}
      }
    }
  } else printf("mpsh: erreur de syntaxe \n");
}


//redirection: entrée standard "<" avec première partie commande et deuxième partie fichier(s)
void redirect_entree(char *line){
  char l[strlen(line)];
  strcpy(l, line); //copie de la ligne telle quelle avant toute modification avec strtok
  char **toks = malloc(sizeof(char*) * TOKSIZE);
  char *tok;
  int ind = 0;
  tok = strtok(line, "<");
  while(tok != NULL){
    toks[ind++] = tok;
    tok = strtok(NULL, "<");
  }
  int compteur = 0; //compter le nombre de "<"
  for(int i = 0; i < strlen(l); i++){
    if(l[i] == '<') compteur++;
  }
  if((ind-1) == compteur){
    trim(toks[ind-2]);
    if(strlen(toks[ind-2]) != 0){ //vérification pour une bonne syntaxe
      trim(toks[ind-1]);
      int fd = open(toks[ind-1], O_RDONLY, S_IRWXU); //lecture
      pid_t pid;   
      pid = fork();
      if(pid == 0){
	if(toks[ind-1] != NULL){
	  if(fd == -1){
	    printf("%s: %s: Aucun fichier ou dossier de ce type\n", toks[0], toks[ind-1]);
	    exit(1);
	  }
	  dup2(fd, 0);
	  close(fd);
	  char cmd[strlen(toks[0])+strlen(toks[ind-1])+1];
	  strcpy(cmd, toks[0]);
	  if(strcmp(toks[0], "cat") == 0){
	    strcat(cmd, SPACE);
	    strcat(cmd, toks[ind-1]);
	  }
	  char **s = split_line(cmd);
	  cmd_alias(nb_arg(s), s);
	  exit(1);
	}
	else {
	  fprintf(stderr, "erreur \n");
	  exit(1);
	}
      } else wait(NULL);
    } else printf("mpsh: erreur de syntaxe \n");
  } else printf("mpsh: erreur de syntaxe \n");
}


//redirection: sortie standard ">" avec première partie commande et deuxième partie fichier(s)
void redirect_sortie(char *line){
  char l[strlen(line)];
  strcpy(l, line); //copie de la ligne telle quelle avant toute modification avec strtok
  char **toks = malloc(sizeof(char*) * TOKSIZE);
  char *tok;
  int ind = 0;
  tok = strtok(line, ">");
  while(tok != NULL){
    toks[ind++] = tok;
    tok = strtok(NULL, ">");
  }
  int compteur = 0; //compter le nombre de ">"
  for(int i = 0; i < strlen(l); i++){
    if(l[i] == '>') compteur++;
  }
  if((ind-1) == compteur){
    trim(toks[ind-2]);
    if(strlen(toks[ind-2]) != 0){ //vérification pour une bonne syntaxe
      trim(toks[ind-1]);
      struct stat s;
      for(int i = 1; i < ind; i++) trim(toks[i]); //supprimer les espaces avant et après
      int descripteur[2]; 
      pid_t pid;    
      char buf;
      char sortie[size*10];
      if(pipe(descripteur) != -1){ //si pipe fonctionne
	pid = fork();
	if(pid > 0){ //père	  
	  for(int i = 1; i < ind; i++){     
	    if(stat(toks[i], &s) == -1){ //si c'est non existant
	      fopen(toks[i], "w+"); //création d'un fichier vide
	    } else { //si existant
	      if((s.st_mode & S_IFDIR) == S_IFDIR){ //si un des arguments est un répertoire alors pas d'exécution
		printf("mpsh: %s: est un dossier \n", toks[i]);
		return; //arrêt
	      } else {
		fopen(toks[i], "w+"); //fichier vidé		
	      }
	    }
	  }
	  close(descripteur[1]); //fermer en écriture
	  int i = 0;
	  while(read(descripteur[0], &buf, 1) > 0){ //1 pour la sortie
	    sortie[i] = buf; //rempli la chaîne sortie caractère par caractère
	    i++;
	    sortie[i]= '\0'; //fin de la chaine de caractère
	  }
	  close(descripteur[0]); //fermer en lecture
	  FILE *fic;
	  fic = fopen(toks[ind-1], "w+");
	  fprintf(fic, "%s", sortie);
	  fclose(fic);
	} else if(pid == 0){
	  dup2(descripteur[1], 1);
	  close(descripteur[1]);
	  char **a;
	  a = split_line(toks[0]);
	  cmd_alias(nb_arg(a), a);
	  wait(NULL);
	  exit(1);
	} else printf("erreur \n");
      }
    } else printf("mpsh: erreur de syntaxe \n");
  } else printf("mpsh: erreur de syntaxe \n");
}


//crée une variable locale
var* init(char *n, char *c){
  var* v = malloc(sizeof(var));
  if(v == NULL){
    return NULL;
  }
  v -> name = strdup(n);
  v -> val = strdup(c);
  v -> next = NULL;
  return v;
}


//récupère la valeur d'une variable locale
char* get_val(char *n){
  if(vars == NULL){
    return NULL;
  } else {
    var* tmp = vars;
    while(tmp != NULL){
      if(strcmp(tmp -> name, n) == 0){
        return tmp -> val; 
      } else {
	tmp = tmp -> next;
      }
    }
  }
  return NULL;
}


//vérifie l'existence d'une variable locale
int contains_name(char *n){
  if(vars == NULL){
    return 0;
  } else {
    var* tmp = vars;
    while(tmp != NULL){
      if(strcmp(tmp -> name, n) == 0){
	return 0;
      } else {
	tmp = tmp -> next;
      }
    }
  }
  return 1;
}


//change la valeur d'une variable locale
int change_val(char *n, char *v){
  if(vars == NULL){
    return 0;
  } else {
    var* tmp = vars;
    while(tmp != NULL){
      if(strcmp(tmp -> name, n) == 0){
        tmp -> val = strdup(v);
	return 0;
      } else {
	tmp = tmp -> next;
      }
    }
  }
  return 1;
}


//rajoute une variable locale
int add_vars(char *line){
  int ind = 0;
  char **tokens = malloc(2*sizeof(char *));
  char *token;
  token = strtok(line, "=");
  while(token != NULL){
    tokens[ind++] = token;
    token = strtok(NULL, "=");
  }
  if(getenv(tokens[0]) != NULL){
    setenv(tokens[0], tokens[1], 1);
    return 0;
  } else {
    if(vars == NULL){
      vars = init(tokens[0], tokens[1]);
      if(vars == NULL) {
	printf("Création de variables locales impossible!");
	return 1;
      }
      return 0;
    } else if(contains_name(tokens[0]) == 0){
      return change_val(tokens[0], tokens[1]);
    } else {
      var* tmp = vars;
      while(tmp -> next != NULL){
	tmp = tmp -> next;
      }
      tmp -> next = init(tokens[0], tokens [1]);
      return 0;
    }
  }
  return 1;
}


//affiche la liste des variables locales
void printlist(){
  if(vars == NULL){
    printf("Pas de variables locales !");
  } else {
    var* p = vars;
    while(p != NULL){
      printf("|%s = %s| ", p -> name, p -> val);
      p = p -> next;
    }
  }
  printf("\n");
}
      
//fait tourner le terminal
void my_shell(){
  char *line;
  char **argv;
  int status;
  int nb;
  do {    
    prompt = getcwd(prompt, size);
    printf("%s$ ", prompt); //affiche le répertoire courant
    line = read_line(); //lit la ligne
    if(line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0'; //supprimer saut de ligne    
    if((strstr(line, "&&") != NULL) || (strstr(line, "||") != NULL)){ //line contenant && ou ||
      connecteurs(line); //fonction auxiliaire
    } else if((strstr(line, "<") != NULL) || (strstr(line, ">") != NULL) || (strstr(line, "2>") != NULL)){ //les redirections
      if(strstr(line, "<") != NULL){ //entrée standard
	redirect_entree(line);
      }
      if(strstr(line, ">") != NULL){ //sortie standard
	redirect_sortie(line);
      }
    } else { //sans connecteurs ni redirections
      argv = split_line(line); //et récupère les arguments
      nb = nb_arg(argv);
      status = cmd_alias(nb, argv);
      varr = status;
      free(line);
      free(argv);
      //printlist();
    }
  } while(status >= 0);
  free(vars);
}


int main(int argc, char **argv){
  my_shell();
}
