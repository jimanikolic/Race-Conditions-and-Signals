# Race-Conditions-and-Signals
Removing race conditions via signals.
Input : Parent takes a single argument, n (3-10), 
- parent sets up arguments and enviornments for the child and forks n times. 
Output: Greeted by pid of each process in reverse order

Ex: 
./parent 3
Process 2 (pid 1917620) greets you.
Process 1 (pid 1917619) greets you.
Process 0 (pid 1917618) greets you.
