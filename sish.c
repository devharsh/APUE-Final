#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <assert.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

static char line[1024];
static int n = 0;
static void split(char* cmd);

static char* args[512];
pid_t pid;
char buf[1024];
char shell_buf[1024];
char* query = NULL;
char* buf_cmd;
char ch;

int status = 0;
int is_c_on = 0;
int is_x_on = 0;
int data = 0;
int cnt = 0;
int startp[2];
int endp[2];

void
sish_help() {
        fprintf(stderr,
        "Usage: %s [-x] [-c command]\n"
        "\t-x            Enable tracing mode\n"
        "\t-c command    Execute the given command\n",
        getprogname());
        exit(EXIT_FAILURE);
}

static int command(int input, int first, int last)
{
 
	/* shell built-in calls */
                if (strcmp(args[0], "echo") == 0) {
                        if(strcmp(args[1], "$$") == 0) {
                                printf("%d\n", (int)getpid());
                        } else if(strcmp(args[1], "$?") == 0) {
                                printf("%d\n", status);
                        } else {
                                printf("%s\n", args[1]);
                        }
                } else if (strcmp(args[0], "cd") == 0) {
                        if(args[1]==NULL) {
                                printf("%s\n", getenv("HOME"));
                                if(chdir(getenv("HOME")) == -1) {
                                        fprintf(stderr, "getenv error: %s\n", strerror(errno));
                                        exit(1);
                                }
                        } else {
                                if(chdir(args[1]) == -1) {
                                        fprintf(stderr, "chdir error: %s\n", strerror(errno));
                                        exit(1);
                                }
                        }
                } else {
	       int pipes[2];
        pipe(pipes);

        if((pid=fork()) == -1) {
                        fprintf(stderr, "shell: can't fork: %s\n", strerror(errno));
                        exit(EX_DATAERR);
        } else if (pid == 0) {
                if (first == 1 && last == 0 && input == 0) {
                        dup2(pipes[1], STDOUT_FILENO );
                } else if (first == 0 && last == 0 && input != 0) {
                        dup2(input, STDIN_FILENO);
                        dup2(pipes[1], STDOUT_FILENO);
                } else {
                        dup2( input, STDIN_FILENO );
                }
		
		
		/*	
		for(int i = 0; i < 512; i++) {
			if(args[i] != NULL) {
				printf("args[%d] = %s\n", i, args[i]);
			}
		}
		*/

                execvp(args[0], args);
		fprintf(stderr, "shell: couldn't exec %s\n", strerror(errno));
		exit(EX_DATAERR);
		}

        if (input != 0)
                close(input);

        close(pipes[1]);

        if (last == 1)
                close(pipes[0]);

        return pipes[0];
	}
}

int 
main(int argc, char** argv) {
	if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
		fprintf(stderr, "signal error: %s\n", strerror(errno));
		exit(1);
	}

	if(getcwd(shell_buf, 1024) == NULL) {
        	fprintf(stderr, "getcwd error: %s\n", strerror(errno));
		exit(1);
	}

    	if(setenv("SHELL", shell_buf, 1) == -1) {
        	fprintf(stderr, "setenv error: %s\n", strerror(errno));
		exit(1);
	}

	while ((ch = getopt(argc, argv, "c:x")) != -1) {
		switch (ch) {
			case 'x':
				is_x_on = 1;
				break;
			case 'c':
				if (optarg) {
					query = optarg;
					is_c_on = 1;
				} else {
					sish_help();
					exit(0);
				}
				break;
			default:
				break;
			}
	}

	while (1) {
		char* cmd;
		int input = 0;
		int first = 1;

		if(!is_c_on) {
			printf("sish$ ");
                	fflush(NULL);

                	if (!fgets(line, 1024, stdin))
                        	return 0;

			cmd = line;
		}

		if(is_c_on) {
			args[0] = query;
			args[1] = NULL;
			execvp(args[0], args);
                	fprintf(stderr, "shell: couldn't exec %s\n", strerror(errno));
                	exit(EX_DATAERR);
		}

		if(is_x_on) {
			fprintf(stderr, "+%s", cmd);
		}

                char* next = strchr(cmd, '|');

                while (next != NULL) {
                	*next = '\0';
			split(cmd);
        		if (args[0] != NULL) {
				if (strcmp(args[0], "exit") == 0)
                        		exit(0);
                		n += 1;
                		input = command(input, first, 0);
        		} else {
				input = 0;
			}

                       	cmd = next + 1;
                       	next = strchr(cmd, '|');
                       	first = 0;
               	}

		split(cmd);
        	if (args[0] != NULL) {
			if (strcmp(args[0], "exit") == 0)
                        	exit(0);
			n += 1;
               		input = command(input, first, 1);
        	} else {
			input = 0;
		}

               	for (int i = 0; i < n; ++i)
                       	wait(NULL);
                n = 0;
        }
        return 0;
}

static void split(char* cmd)
{
        while (isspace(*cmd)) {
                cmd++;
        }
        char* next = strchr(cmd, ' ');
        int i = 0;

        while(next != NULL) {
                next[0] = '\0';
                args[i] = cmd;
                ++i;
                cmd = next + 1;
                while (isspace(*cmd)) {
                        cmd++;
                }
                next = strchr(cmd, ' ');
        }

        if (cmd[0] != '\0') {
                args[i] = cmd;
                next = strchr(cmd, '\n');
                next[0] = '\0';
                ++i;
        }

        args[i] = NULL;
}

