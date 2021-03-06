/*
 **
 **
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

volatile sig_atomic_t usr1Happened = 0;

int main(void) {
	void usr1_handler(int);
	void parent_handler(int);

	struct sigaction sa;
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = &usr1_handler;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	int numChildren;
	printf("Enter the number of children: ");
	scanf(" %d", &numChildren);

	int numFork = numChildren;
	int i;
	bool exitStatus = false;
	pid_t pids[10];

	printf("PID = %d: Parent running...\n", getppid());
	for (i = 0; i < numFork; ++i) {
		usr1_happened = 0;
		switch (pids[i] = fork()) {
		case -1:
			perror("Child(ren) failed, parent exiting.\n");
			exit(-1);
		case 0:
			printf("PID = %d: Child running...\n", getpid());
			sigaction(SIGUSR1, &sa, NULL);
			if (usr1_happened == 1)
				exit(0);
		}
	}

	int returnVal;
	pid_t pid;
	while (numFork > 0) {
		pid = wait(&returnVal);
		--numFork;
	}
	sigaction(SIGUSR1, &sa, NULL); //not right

	return 0;
}

void usr1_handler(int signo) {
	usr1Happened = 1;
	write("PID = %d: Child received USR1.\n", getpid());
	write("PID = %d: Child exiting.\n", getpid());
}

void parent_handler() {
	printf("PID = %d: Child(ren) finished, parent exiting.\n", getppid());
}
