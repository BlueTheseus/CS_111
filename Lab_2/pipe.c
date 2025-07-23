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

	int rc = fork();
	if (rc < 0) {
		// FORK ERROR
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (rc == 0) {
		// CHILD
		printf("child PID: %d\n", (int) getpid());
		return_status = execlp("ls", "ls");
	} else {
		// PARENT
		wait(&status);
		printf("parent with PID %d of child %d\n", (int) getpid(), rc);
	}
	return return_status;
}
