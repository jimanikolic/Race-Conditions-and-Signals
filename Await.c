#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h> // for getenv(), exit()

volatile sig_atomic_t eventdone = 0;

void DieWithError(char *msg) {
  perror(msg); // expect errno to be set when this is called.
  exit(1);
}

void usr1handler(int sig) {
    eventdone = 1;
}


void setup_handler(int sig) {
    struct sigaction myaction;

    myaction.sa_handler = usr1handler;
    myaction.sa_flags = 0;

    // block everything while handler is running
    if (sigfillset(&myaction.sa_mask) < 0){
        DieWithError("sigfillset");
    }

    /* third argument is NULL b/c we don't care about previous disposition */
    if (sigaction(sig,&myaction,NULL) < 0) {
        DieWithError("sigaction");
    }
}

/* wait for eventdone to become true.
 * Precondition: handler already established */
void await_event(void) {
    sigset_t blockmask, oldmask;

    // create a mask containing all signals
    if ((sigfillset(&blockmask) < 0)) {
        DieWithError("sigfillset");
    }

    // block everything - blunt instrument, but simpler
    if (sigprocmask(SIG_BLOCK,&blockmask,&oldmask) < 0) {
        DieWithError("sigprocmask");
    }

    /* SIGUSR1 (and everything) is now blocked - no race condition */
    while (!eventdone) { // handler has not run yet
        sigsuspend(&oldmask); // unblock everything and pause, all at once.
    }
    /* SIGUSR1 was handled */
    // restore the original set of blocked signals
    if (sigprocmask(SIG_SETMASK,&oldmask,NULL) < 0) {
        DieWithError("sigprocmask (after loop)");
    }
}
