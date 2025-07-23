#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int status = 0;
	int return_status = 0;

	printf("hello from PID %d\n", (int) getpid());

	if (argv[1] == NULL) {
		fprintf(stderr, "%s: provide at least one argument\n", argv[0]);
	};

	for (int i = 1; i < argc; i++) {
		printf("\nargv[%d]: %s\n", i, argv[i]);
		int childPID = fork();
		if (childPID < 0) {
			// FORK ERROR
			fprintf(stderr, "fork failed\n");
			exit(1);
		} else if (childPID == 0) {
			// CHILD
			return_status = execlp(argv[i], argv[i], (char*) NULL);
			fprintf(stderr, "execlp failed\n");
			exit(return_status);
		} else {
			// PARENT
			wait(&status);
			//printf("\nall commands finished\n");
		}
	}
	printf("\nall commands finished\n");

/*	int rc = fork();
	if (rc < 0) {
		// FORK ERROR
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (rc == 0) {
		// CHILD
		printf("child PID: %d\n", (int) getpid());
		return execlp("ls", "ls");
	} else {
		// PARENT
		wait(&status);
		printf("parent with PID %d of child %d\n", (int) getpid(), rc);
	}*/
	return return_status;
}
