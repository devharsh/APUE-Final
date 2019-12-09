#include "sish.h"

static void sig_int();

int
main(int argc, char** argv) {
	char buf[1024];
	char* query = NULL;
	char ch;

	pid_t pid;
	int status;
	int is_x_on;

	if (signal(SIGINT, sig_int) == SIG_ERR) {
		fprintf(stderr, "signal error: %s\n", strerror(errno));
		exit(1);
	}

	while ((ch = getopt(argc, argv, "c:x")) != -1) {
		switch (ch) {
            		case 'x':
                		is_x_on = 1;
				break;
            		case 'c':
                		if (optarg)
                			query = optarg;
				else
					sish_help();
                		break;
            		case '?':
            			sish_help();
            			exit(1);
			default:
 				sish_help();               		
                		break;
        	}
    	}

	if(is_x_on) {}
	if(query != NULL) {}

	while (getinput(buf, sizeof(buf))) {
		buf[strlen(buf) - 1] = '\0';

		if((pid=fork()) == -1) {
			fprintf(stderr, "shell: can't fork: %s\n",
					strerror(errno));
			continue;
		} else if (pid == 0) {
			/* child */
			execlp(buf, buf, (char *)0);
			fprintf(stderr, "shell: couldn't exec %s: %s\n", buf,
					strerror(errno));
			exit(EX_DATAERR);
		}

		if ((pid=waitpid(pid, &status, 0)) < 0)
			fprintf(stderr, "shell: waitpid error: %s\n",
					strerror(errno));
	}

	exit(EX_OK);
}

void
sig_int() {
	printf("\nCaught SIGINT!\n");
}
