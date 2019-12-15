#include "sish.h"

int
main(int argc, char** argv) {
	char buf[1024];
	char* query = NULL;
	char ch;

	pid_t pid;
	int status = 0;
	int is_x_on = 0;

	if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
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

	if(query != NULL) {}

	while (getinput(buf, sizeof(buf))) {
		buf[strlen(buf) - 1] = '\0';

		if(is_x_on) {
			printf("+%s\n", buf);
		}

		if(strcmp("echo $$", buf) == 0) {
			printf("%d\n", (int)getpid());
			status = 0;
			continue;
		} else if(strcmp("echo $?", buf) == 0) {
			printf("%d\n", status);
			status = 0;
			continue;
		} else {
			if((pid=fork()) == -1) {
				fprintf(stderr, "shell: can't fork: %s\n",
					strerror(errno));
				continue;
			} else if (pid == 0) {
				execlp(buf, buf, (char *)0);
				fprintf(stderr, "shell: couldn't exec %s: %s\n", buf,
					strerror(errno));
				exit(EX_DATAERR);
			}
		}

		if ((pid=waitpid(pid, &status, 0)) < 0)
			fprintf(stderr, "shell: waitpid error: %s\n",
					strerror(errno));
	}

	exit(EX_OK);
}
