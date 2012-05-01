#include "http.h"
#include <string.h>

int parsereq(char req[], int size, char **url, int *url_len) {
        char *pos = strstr(req, "GET");
        int len;
        if (strstr(req, "HTTP/1.0") == NULL)
                return -1;
        if (pos == NULL || pos != req)
                return -1;
        pos+=3; /* skip GET */
        pos++; /* skip space */
        *url = pos;
        while (*pos != ' ') {
                pos++;
        }
        *url_len = (int) pos - (int) *url;
        return 0;
}

int parseurl(char url[], int *host_len, char **host, char **dir) {
        int i = 0, cmp = 0;
        while (i < 3) {
                if (*url == '/') {
                        i++;
                }
                if (i == 2 && !cmp) {
                        *host = url+1;
                        cmp = 1;
		}
                url++;
        }
        url--;
        *dir = url;
        *host_len = (int) url - (int) *host;
}

