#include <stdio.h>
#include <sys/types.h> // for pid_t
#include <unistd.h> // for pid_t
#include <stdlib.h> // for getenv(), exit()
#include "proj5.h"
#include <string.h>
#include <sys/wait.h>
#include <time.h>

//Init Functions
void setup_handler(int sig);
void await_event(void);

int main(int argc, char *argv[]) {
  int num = atoi(getenv(WRITEFDKEY)); //getenv() and atoi() for file descriptor write end
  int pid;
  int sigPid; //send this PID
  char pidArr[9];
  srandom(time(NULL)); //for random number  
  //Parse arguments to get the PID of the process to be sent sigusr1.
  pid = getpid();
  sigPid = atoi(argv[PIDARG]);
  sprintf(pidArr, "%d", pid);

  //Call setup_handler() to, well, set up the signal handler.
  setup_handler(SIGUSR1);

  //Write its process ID (as a 4-byte integer) to the pipe.
  if(write(num, pidArr, strlen(pidArr)+2) <1) {
    printf("fail in child write\n");
    exit(0);
  }
//Call await_event to block until sigusr1 arrives.
  close(num);
  await_event();

  //Print its greeting.
  printf("Process %s (pid %d) greets you.\n", argv[ORDARG], pid);

  //Send sigusr1 to the PID given in its third argument.
  kill(sigPid, SIGUSR1);

  //Exit with a random status between 100 and 200.
  exit((100 + (random() % 101)));
