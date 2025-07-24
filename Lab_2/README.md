## UID: 805796976


## Pipe Up

Here is a program which allows you to string together the output of one command
to the input of the next supplied command.


## Building

To build, run `make`. An executable file named `pipe` will be created in the
current directory.


## Running

```
use: pipe [commands]
```

Any non-zero amount of commands to be piped together is accepted. Below you can
find some examples of running the command on this directory right after it was
built.

Example of no output:
```shell
# pipe
pipe: provide at least one command.
```

Example output of one command:
```shell
# pipe ls
Makefile  pipe  pipe.c  pipe.o  README.md  test_lab1.py
```

Example output of two commands:
```shell
# pipe ls wc
      6       6      51
```


## Cleaning up

To clean up all binary files, run `make clean`.
