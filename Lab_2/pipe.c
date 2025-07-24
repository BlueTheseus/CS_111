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
		fprintf(stderr, "error creating pipe\n");
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
				fprintf(stderr, "error closing unused readable pipe\n");
				exit(1);
			}
			dup2(pipefd[1], STDOUT_FILENO); // Link stdout to write to the pipe
			exit_status = execlp(argv[1], argv[1], (char *) NULL);
			fprintf(stderr, "execlp failed\n");
			exit(exit_status);

		default: // PARENT
			//wait(&exit_status);
			if (close(pipefd[1]) == -1) {
				fprintf(stderr, "error closing unused writeable pipe\n");
				exit(1);
			}
			wait(&exit_status);
	}
	int previous_pipe_read = pipefd[0];

	// Handle pipes for all middle programs
	for (int i = 2; i < argc-1; i++) {
		// Create a new pipe
		if (pipe(pipefd) == -1) {
			fprintf(stderr, "error creating new pipe\n");
			exit(1);
		}

		// Fork
		childPID = fork();
		switch (childPID) {
			case -1: // FORK ERROR
				fprintf(stderr, "fork failed\n");
				exit(1);

			case 0:  // CHILD
				if (close(pipefd[0]) == -1) {
					fprintf(stderr, "error closing unused readable pipe\n");
					exit(1);
				}
				dup2(previous_pipe_read, STDIN_FILENO); // Link stdin to read previous pipe
				dup2(pipefd[1], STDOUT_FILENO); // Link stdout to write to next pipe
				exit_status = execlp(argv[i], argv[i], (char*) NULL);
				fprintf(stderr, "execlp failed\n");
				exit(exit_status);

			default: // PARENT
				wait(&exit_status);
				if (close(previous_pipe_read) == -1) {
					fprintf(stderr, "error closing unused readable pipe\n");
					exit(1);
				}
				//wait(&exit_status);
				if (close(pipefd[1]) == -1) {
					fprintf(stderr, "error closing unused writeable pipe\n");
					exit(1);
				}
				//wait(&exit_status);
				previous_pipe_read = pipefd[0];
		}
	}

	// Have last program get input from pipe
	childPID = fork();
	switch (childPID) {
		case -1: // FORK ERROR
			fprintf(stderr, "fork failed\n");
			exit(1);

		case 0:  // CHILD
			dup2(previous_pipe_read, STDIN_FILENO); // Link stdin to read from the pipe
			exit_status = execlp(argv[argc-1], argv[argc-1], (char *) NULL);
			fprintf(stderr, "execlp failed\n");
			exit(exit_status);

		default: // PARENT
			if (close(previous_pipe_read) == -1) {
				fprintf(stderr, "error closing unused readable pipe\n");
				exit(1);
			}
			wait(&exit_status);
			return WEXITSTATUS(exit_status);
	}
	return 0;
}
