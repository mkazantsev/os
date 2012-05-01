#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
	FILE *fp[2];
	int i, n;
	
	p2open("sort -n", fp);
	
	srand(time(NULL));
	
	for (i = 0; i < 100; i++)
	    fprintf(fp[0], "%d\n", rand() % 100);
	    
	fclose(fp[0]);
	
	i = 0;
	while(fscanf(fp[1], "%d", &n) == 1) {
	    printf("%2d ", n);
	    i++;
	    if (i % 10 == 0)
		printf("\n");
	}
	
	p2close(fp);
	return 0;
}
