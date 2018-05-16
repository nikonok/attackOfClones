# Attack of clones

--------------------------------------
## Task

Runs two copies of the program, we call them A and B.

A passes B its PID and starts the child process A* with the fork () function.
      
B kills the main process A, and process A* becomes the main (replaces A).        
       
Then the opposite is true: B passes A (i.e. the former A*) his PID and creates his copy of B* through fork (), and then a kills the main process B.
        
And so on to infinity

--------------------------------------

## How to compile

Use `make` command. And output will be main.o

## How to run

**Check if `/tmp/myfifo` exists , it must be removed for the program to work correctly**

__better use `rm -rf /tmp/myfifo` before running__

Use `./main.o {name}`, you should enter programm name (example: `./main.o A`)

## How to see what's happening

You can look at log file (named log{name}, example: logA).
Also you can use `pgrep main`, then you will see PID of running processes. 
There will be from 2 to 5 numbers: it depends on how works `kill {pid}` on you shell and on program state.

## How to stop this

Use `killall main.o`.
