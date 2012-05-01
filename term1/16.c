#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
	char buf;
	int i;
	struct termios oldterm, newterm;
	
	tcgetattr(0, &oldterm);
	
	newterm = oldterm;
	newterm.c_lflag &= ~(ICANON | IEXTEN | ISIG);
	newterm.c_iflag &= ~IXON;
	newterm.c_cc[VMIN] = 1;
	newterm.c_cc[VTIME] = 0;
	
	printf("To Y or not to Y?\n%c", newterm.c_cc[VLNEXT]);
	
	tcsetattr(0, TCSANOW, &newterm);
	read(0, &buf, 1);
	tcsetattr(0, TCSANOW, &oldterm);
	
	printf("\n");
	if (buf == 'y' || buf == 'Y')
		printf("Yes\n");
	else if (buf == 'n' || buf == 'N')
		printf("No\n");
	else
		printf("Unknown answer\n");
	return 0;
}
