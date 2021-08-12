SEEsh

Sources referenced:
https://brennan.io/2015/01/16/write-a-shell-in-c/
https://stackoverflow.com/questions/2085302/printing-all-environment-variables-in-c-c
https://www.tutorialspoint.com/write-a-c-program-that-does-not-terminate-when-ctrlplusc-is-pressed

Things working correctly:
- Reading, interpreting and printing from .SEEshrc at user's HOME.
- In-built functions: cd (partially: described in bugs below),
  pwd, help, set, unset, exit
- Running an executable (partially: described in bugs below)
- Terminating shell with ctrl+D and not ctrl+C (terminating child with ctrl+C)
- Error handling on incorrect input

Known bugs:
- Two '?' in one line after setting and unsetting env variables.
- Multiple '?' in a line if executable is not found
- Typing just cd does not navigate to home (was working previously)
- Can only run executable using ./doublylinkedlist from .SEEshrc. (Was
  able to run from command line before).
  
makefile:
Compile - make
Clean - make clean
Run - make run
