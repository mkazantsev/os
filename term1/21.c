#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int count = 0;

void sigint_handler(int signum) {
	write(1, "\a", 1);
	count++;
}

void sigquit_handler(int signum) {
	printf("\nTotal: %d\n", count);
	exit(0);
}

int main(void) {
	struct sigaction sigint_action;

	sigint_action.sa_handler = sigint_handler;
	sigint_action.sa_flags = 0;
	
	sigaction(SIGINT, &sigint_action, NULL);
	signal(SIGQUIT, sigquit_handler);
	while(1);
	return 0;
}
