#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
extern char *tzname[];
extern time_t timezone;

int main(void)
{
    time_t now;
    struct tm *sp;

    time(&now);

    tzset();
    //now += timezone;
    now -= 8*60*60;

    printf("%s", ctime(&now));

    sp = gmtime(&now);
    printf("%d/%d/%02d %d:%02d\n",
	sp->tm_mon + 1, sp->tm_mday,
	sp->tm_year, sp->tm_hour,
	sp->tm_min);
    return 0;
}
