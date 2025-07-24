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

	//printf("argc: %d\n", argc);
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
				fprintf(stderr, "error closing unused readable pipe");
				exit(1);
			}
			dup2(pipefd[1], STDOUT_FILENO); // Link stdout to write to the pipe
			exit_status = execlp(argv[1], argv[1], (char *) NULL);
			fprintf(stderr, "execlp failed\n");
			exit(exit_status);

		default: // PARENT
			//wait(&exit_status);
			if (close(pipefd[1]) == -1) {
				fprintf(stderr, "error closing unused writeable pipe");
				exit(1);
			}
			wait(&exit_status);
	}
	int previous_pipe_read = pipefd[0];

	// Handle pipes for all middle programs
	for (int i = 2; i < argc-1; i++) {
		printf("loop iteration %d\n", i);

		// Create a new pipe
		//previous_pipe_read = pipefd[0];
		if (pipe(pipefd) == -1) {
			fprintf(stderr, "pipe error");
			exit(1);
		}
		printf("...created new pipe\n");

		// Fork
		childPID = fork();
		switch (childPID) {
			case -1: // FORK ERROR
				fprintf(stderr, "fork failed\n");
				exit(1);

			case 0:  // CHILD
				printf("...child created\n");
				if (close(pipefd[0]) == -1) {
					fprintf(stderr, "error closing unused readable pipe");
					exit(1);
				}
				dup2(previous_pipe_read, STDIN_FILENO); // Link stdin to read previous pipe
				dup2(pipefd[1], STDOUT_FILENO); // Link stdout to write to next pipe
				exit_status = execlp(argv[i], argv[i], (char*) NULL);
				fprintf(stderr, "execlp failed\n");
				exit(exit_status);

			default: // PARENT
				printf("...parent waiting\n");
				wait(&exit_status);
				if (close(previous_pipe_read) == -1) {
					fprintf(stderr, "error closing unused readable pipe");
					exit(1);
				}
				//wait(&exit_status);
				if (close(pipefd[1]) == -1) {
					fprintf(stderr, "error closing unused writeable pipe");
					exit(1);
				}
				//wait(&exit_status);
				previous_pipe_read = pipefd[0];
				printf("...parent done waiting\n");
		}
	}

	// Have last program get input from pipe
	childPID = fork();
	switch (childPID) {
		case -1: // FORK ERROR
			fprintf(stderr, "fork failed\n");
			exit(1);

		case 0:  // CHILD
			/*if (close(pipefd[1]) == -1) {
				fprintf(stderr, "error closing unused writeable pipe");
				exit(1);
			}*/
			//dup2(pipefd[0], STDIN_FILENO); // Link stdin to read from the pipe
			dup2(previous_pipe_read, STDIN_FILENO); // Link stdin to read from the pipe
			exit_status = execlp(argv[argc-1], argv[argc-1], (char *) NULL);
			fprintf(stderr, "execlp failed\n");
			exit(exit_status);

		default: // PARENT
			if (close(previous_pipe_read) == -1) {
				fprintf(stderr, "error closing unused readable pipe");
				exit(1);
			}
			/*if (close(pipefd[1]) == -1) {
				fprintf(stderr, "error closing unused writeable pipe");
				exit(1);
			}*/
			wait(&exit_status);
			return WEXITSTATUS(exit_status);
	}
	return 0;
}
