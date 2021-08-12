#define _GNU_SOURCE

#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>

//\t tab, \r return, \n rew line, \a alert
#define ARG_DELIMITER " \t\r\n\a"

//Prototypes
void sigint_handler(int  signum);
void seesh_startup();
void main_loop();
char* seesh_read_line();
char** seesh_split_line(char* line);
int seesh_execute(char** args);

//Shell built-in commands
int seesh_cd(char** args);
int seesh_pwd(char** args);
int seesh_help(char** args);
int seesh_setvar(char** args);
int seesh_unsetvar(char** args);
int seesh_exit(char** args);

char* builtin_str[] = {"cd", "pwd", "help", "set", "unset", "exit"};

int(*builtin_func[])(char**) = {&seesh_cd, &seesh_pwd, &seesh_help, &seesh_setvar, &seesh_unsetvar, &seesh_exit};

int seesh_num_builtins(){
  return sizeof(builtin_str)/sizeof(char*);
}

//Load .seeshrc and run commands in it on startup
void seesh_startup(){

  char* home = getenv("HOME");

  //printf("This is home: %s\n", home);

  char* filename = "/.SEEshrc";
  char* file_location = strcat(home, filename);

  //printf("This is file location: %s\n", file_location);

  char currentline[64];

  char** args;

  //Access .SEEshrc in user's home directory
  FILE* file = fopen(file_location, "r");

  printf("Reading from .SEEshrc (if it exists)\n");

  if(file != NULL){
    while(fgets(currentline, sizeof(currentline), file) != NULL){
      //Print each line to console
      puts(currentline);
      //puts(currentline);

      //Execute line
      args = seesh_split_line(currentline);
      seesh_execute(args);

      printf("\n");
    }

    fclose(file);
  }
  else{
    fprintf(stderr, "Unable to find/open .SEEshrc\n");
  }
}

void sigint_handler(int signum){
  signal(SIGINT, sigint_handler);
}

//Main loop thats prompts user to enter commands
void main_loop(){

  char* line;
  char** args;
  int status;

  //Do not stop shell with ctrl+C
  signal(SIGINT, sigint_handler);

  do{

    printf("? ");
    line = seesh_read_line();
    args = seesh_split_line(line);
    status = seesh_execute(args);

    //printf("Trying to free memory, 4\n");

    free(line);
    free(args);

    //printf("Memory freed, 5\n");

  }while(status);
}

char* seesh_read_line(){

  char* line = malloc(512 * sizeof(char));
  //size_t buffer_size = 0;

  if(fgets(line, sizeof(line), stdin) == NULL){
    printf("\n");
    exit(0);
  }

  //getline(&line, &buffer_size, stdin);

  //printf("END OF READLINE, 1\n");

  return line;
}

char** seesh_split_line(char* line){

  int position = 0;
  char** args = malloc(64 * sizeof(char*));
  char* arg;

  if(!args){
    fprintf(stderr, "SEEsh: memory allocation error\n");
  }

  //printf("Successfull malloc, 2\n");

  arg = strtok(line, ARG_DELIMITER);
  while(arg != NULL){
    args[position] = arg;
    position++;

    arg = strtok(NULL, ARG_DELIMITER);
  }
  args[position] = NULL;

  //printf("END OF SPLITLINE, 3\n");

  return args;
}

int seesh_launch(char** args){

  pid_t pid;
  int status;

  pid = fork();
  if(pid == 0){
    //Child process
    if(execvp(args[0], args) == -1){ //execvp uses the PATH variable to locate and run the executable if it exists
      perror("SEEsh");
    }
    exit(EXIT_FAILURE);
  }else if(pid < 0){
    //Error forking
    perror("SEEsh");
  }else{
    //Parent process
    do{
      waitpid(pid, &status, WUNTRACED);
    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int seesh_execute(char** args){

  if(args[0] == NULL){
    //An empty command was entered.
    return 1;
  }

  for(int i = 0; i < seesh_num_builtins(); i++){
    if(strcmp(args[0], builtin_str[i]) == 0){
      return (*builtin_func[i])(args);
    }
  }

  return seesh_launch(args);
}

/*Built-in commands implementation*/

/*Change current working directory*/
int seesh_cd(char** args){

  if(args[1] == NULL){

    /*if(chdir(getenv("HOME")) != 0){
      perror("SEEsh");
    }*/

    chdir(getenv("HOME"));

  }else{

    if(chdir(args[1]) != 0){
      perror("SEEsh");
    }
  }
  return 1;
}

/*Print the current working directory*/
int seesh_pwd(char** args){

  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  printf("%s\n", cwd);

  return 1;
}

/*Print all built-in commands available with SEEsh*/
int seesh_help(char** args){

  printf("Samarth's SEEsh\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built-in:\n");

  for(int i = 0; i < seesh_num_builtins(); i++){
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the main command interface for information on other programs.\n");

  return 1;
}

/*Set the value for an environment variable*/
int seesh_setvar(char** args){

  extern char** environ;
  int i = 1;
  char* env = *environ;
  char* emptyString = "";

  //if arg is just set, print all environment variables
  if(args[1] == NULL){
    for(; env; i++){
      printf("%s\n", env);
      env = *(environ+i);
    }
  }

  //if only var name, create var if not present and set value to empty string
  else if(args[1] != NULL && args[2] == NULL){
    //if exists, override to empty string. else create and set to empty string
    if(setenv(args[1], emptyString, 0) == -1){
      perror("SEEsh");
    }
  }

  else if(args[1] != NULL && args[2] != NULL){
    //if exists, set to new passed value. else create and set to passed value
    if(setenv(args[1], args[2], 1) == -1){
      perror("SEEsh");
    }
  }

  //Any extra args entered after set (more than 2) are ignored

  return 1;
}

/*Unset the value for an environment variable*/
int seesh_unsetvar(char** args){

  //destroy the passed variable
  //if passed variable does not exist, the environment remains unchanged
  if(unsetenv(args[1]) == -1){
    perror("SEEsh");
  }

  return 1;
}

/*Exit from SEEsh*/
int seesh_exit(char** args){
  return 0;
}

int main(int argc, char** argv){

  //Read execute commands from .SEEshrc and print to console
  seesh_startup();

  //main loop waiting for user input
  main_loop();

  return EXIT_SUCCESS;
}
