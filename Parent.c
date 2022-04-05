#include <sys/types.h> // for pid_t
#include <unistd.h> // for pid_t
#include <stdlib.h> // for getenv(), exit()
#include <string.h>
#include <sys/wait.h>
#include "const.h"
#define MAX_STR 64
#define MAX_BUF 10
#define WRITE_END 1
#define READ_END 0

//Init function await.cs
void setup_handler(int sig);
void await_event(void);

int main(int argc, char *argv[]) {
  int nprocs;
  int i;
  int status;
  int p[2];
  int cPid[MAX_PROC+1];// pids of children + 1 for parent
  int bufPid[MAX_PROC];
  int testPid = 0; //testing pid
  char key[MAX_STR]; // for env
  char *args[NUMARGS]; // prog name, ord, prev proc, null
  char argArr[NUMARGS][MAX_STR];
  char *envp[2]; // environment
  char *buf[MAX_BUF];
  char pipeArr[MAX_BUF][9];
  for(int i = 0; i < MAX_BUF; i++){
      buf[i] = pipeArr[i];
  }

  for (i = 0; i < NUMARGS; i++) {
  args[i] = argArr[i];
  //Print its process ID to standard output.
  cPid[0] = getpid();
  printf("Parent process ID = %d\n", cPid[0]);

  //Check and parse argument to get n, the number of children.
  if (argc != 2) {
      fprintf(stderr,"Error: wrong number of arguments: %d\n",argc);
      exit(1);
  }
  /* argc == 2 */
  nprocs = atoi(argv[1]);
  if (nprocs < MIN_PROC || nprocs > MAX_PROC) {
    fprintf(stderr,"Error: argument must be in [%d,%d]\n",MIN_PROC,MAX_PROC);
    exit(1);
    }

  //Call setup_handler() to set up the signal handler.
  setup_handler(SIGUSR1);

  //Create pipe()
  pipe(p);

  //Establish Enviornment similar to forker.c
  envp[0] = key;
  envp[1] = NULL;
  snprintf(key, MAX_STR, "%s=%d", WRITEFDKEY, p[1]); //WRITEFDNO=x

  //Setup Args
  strcpy(args[0], PROGNAME);
  args[3] = NULL;
    for (i = 1; i <= nprocs; i++) {
      sprintf(args[ORDARG], "%d", (i-1)); //args[1]
      sprintf(args[PIDARG], "%d", cPid[(i-1)]); //args[2]

      cPid[i] = fork(); //record pid of each process in an array
      if (cPid[i] < 0) {
          perror("fork() failed");
          exit(7);
        }

          if (cPid[0] == getpid()) {//parent
          }
            else {//child
                close(p[READ_END]);
                if (execve(PROGNAME, args, envp) < 0) {
                  perror("execve failed.");
                  exit(21);
                }
            }
        }

  //Read from the pipe each time verifying that it is one of the process IDs.
  for(int i = 0; i < nprocs; i++){
      read(p[READ_END], buf[i], 9);
      bufPid[i] = atoi(buf[i]);
      if(testPid < bufPid[i]){
         testPid = bufPid[i];
      }
  }

  //Send sigusr1 to process n −1, using kill(), block until SIGUSR1
    kill(testPid, SIGUSR1);
    await_event();

  //Reap the children from forker.c
    for (i = 0; i < nprocs; i++) {
        pid_t pid = wait(&status);

        if (pid < 0) {
            perror("wait returned -1:");
           exit(10);
        }

        printf("Process %d: ", pid);

        if (WIFEXITED(status)) {
            printf("ended normally, with status %d.\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("was terminated via signal %d.\n", WTERMSIG(status));
        } else {
            printf("unknown state change (can't happen?) status = %x\n", status);
        }
    }
    return 0;
}
                                                                   105,1         83
