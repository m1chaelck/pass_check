#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];
  int status;
  int fd[2];

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }

  pipe(fd);

  int child_pid = fork();
  if(child_pid >0){
    close(fd[0]);
    write(fd[1], user_id, MAX_PASSWORD);
    write(fd[1], password, MAX_PASSWORD);
    if(wait(&status) != -1){
      if (WIFEXITED(status)) {
        switch(WEXITSTATUS(status)){
          case 0:
            printf("%s\n", SUCCESS);
            break;
          case 2:
            printf("%s\n", INVALID);
            break;
          case 3:
            printf("%s\n", NO_USER);
            break;
        }
      }
    }

    close(fd[1]);
  }

  else if(child_pid == 0){
    dup2(fd[0], fileno(stdin));
    close(fd[1]);
    close(fd[0]);
    execlp("./validate", "validate", NULL);
  }

  return 0;
}
