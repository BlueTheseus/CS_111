#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int exit_status;
	int pipefd[2];
	int childPID;

	if (argv[1] == NULL) {
		fprintf(stderr, "%s: provide at least one command.\n", argv[0]);
		exit(1);
	};

	printf("hello from PID %d\n", (int) getpid());

	// Only one command to run
	if (argc == 2) {
		childPID = fork();
		switch (childPID) {
			case -1: // FORK ERROR
				fprintf(stderr, "fork failed\n");
				exit(1);

			case 0:  // CHILD
				exit_status = execlp(argv[1], argv[1], (char *) NULL);
				fprintf(stderr, "execlp failed\n");
				exit(exit_status);

			default: // PARENT
				wait(&exit_status);
				return WEXITSTATUS(exit_status);
		}
	}

	// Create a pipe -- 0=readable, 1=writeable
	if (pipe(pipefd) == -1) {
		fprintf(stderr, "error creating pipe");
		exit(1);
	}

	// Have first program output to pipe
	childPID = fork();
	switch (childPID) {
		case -1: // FORK ERROR
			fprintf(stderr, "fork failed\n");
			exit(1);

		case 0:  // CHILD
			if (close(pipefd[0]) == -1) {
				fprintf(stderr, "error closing unused pipe input");
				exit(1);
			}
			dup2(pipefd[1], STDOUT_FILENO); // Link stdout to write to the pipe
			exit_status = execlp(argv[1], argv[1], (char *) NULL);
			fprintf(stderr, "execlp failed\n");
			exit(exit_status);

		default: // PARENT
			if (close(pipefd[1]) == -1) {
				fprintf(stderr, "error closing unused pipe output");
				exit(1);
			}
			dup2(pipefd[0], STDIN_FILENO);
			wait(&exit_status);
			execlp("cat", "cat", (char *) NULL);
	}



	/*for (int i = 1; i < argc; i++) {
		printf("\nargv[%d]: %s\n", i, argv[i]);

		// Create a pipe
		if (pipe(pipefd) == -1) {
			fprintf(stderr, "pipe error");
			exit(1);
		}

		// Fork
		childPID = fork();
		switch (childPID) {
			case -1: // FORK ERROR
				fprintf(stderr, "fork failed\n");
				exit(1);

			case 0:  // CHILD
				if (close(pipefd[1]) == -1) {
					fprintf(stderr, "error closing unused write-end");
					exit(1);
				}
				dup2(pipefd[]);
				int return_status = execlp(argv[i], argv[i], (char*) NULL);
				fprintf(stderr, "execlp failed\n");
				exit(return_status);

			default: // PARENT
				wait(&status);
		}
	}*/

	printf("\nall commands finished\n");

	return 0;
}
