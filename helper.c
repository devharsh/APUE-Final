#include "sish.h"

char *
getinput(char *buffer, size_t buflen) {
	printf("sish$ ");
	return fgets(buffer, buflen, stdin);
}

void
sish_help() {
        fprintf(stderr,
        "Usage: %s [-x] [-c command]\n"
        "\t-x            Enable tracing mode\n"
        "\t-c command    Execute the given command\n",
        getprogname());
        exit(EXIT_FAILURE);
}
