#include "sish.h"

int
main(int argc, char** argv) {
	char buf[1024];
	char* args[1024];
	char* query = NULL;
	char* buf_cmd;
	char ch;

	pid_t pid;
	
	int status = 0;
	int is_x_on = 0;
	int data = 0;
	int cnt = 0;
	int startp[2];
	int endp[2];

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

		if(is_x_on) {}
		printf("+%s\n", buf);

		if(strcmp("echo $$", buf) == 0) {
			printf("%d\n", (int)getpid());
			status = 0;
			continue;
		} else if(strcmp("echo $?", buf) == 0) {
			printf("%d\n", status);
			status = 0;
			continue;
		} else {
			char* bufspace = buf;
			char* cur_cmd = strchr(bufspace, '|');

			while(isspace(*bufspace)) {
				bufspace++;
			}
			buf_cmd = strchr(bufspace, ' ');
			
			while(buf_cmd != NULL) {
				buf_cmd[0] = '\0';
				args[cnt] = bufspace;
				cnt++;
				
				bufspace = buf_cmd + 1;
				while(isspace(*bufspace)) {
					bufspace++;
				}
				buf_cmd = strchr(bufspace, ' ');
			}

			if(bufspace[0] != '\0') {
				args[cnt] = bufspace;
				buf_cmd = strchr(bufspace, '\n');
				buf_cmd = '\0';
				cnt++;
			} 
			args[cnt] = NULL;

			/* run first program */
			pipe(startp);

			if((pid=fork()) == -1) {
				fprintf(stderr, "shell: can't fork: %s\n", strerror(errno));
				continue;
			} else if(pid == 0) {
				dup2(startp[1], STDOUT_FILENO);
				execvp(args[0], args);
				fprintf(stderr, "shell: couldn't exec %s: %s\n", buf, strerror(errno));
				exit(EX_DATAERR); 
			}
			
			if(data != 0) {
				close(data);
			}
			close(startp[1]);

			/* run intermediates between start and end */ 							
			while(cur_cmd != NULL) {
				int pipes[2];
				pipe(pipes);

				if((pid=fork()) == -1) {
				} else if(pid == 0) {
					dup2(data, STDIN_FILENO);
					dup2(pipes[1], STDOUT_FILENO);
					execvp(args[0], args);
				}

				if(data != 0) {
					close(data);
				}
				close(pipes[1]);
			}

			/* run last program */
			pipe(endp);

			if((pid=fork()) == -1) {
			} else if(pid == 0) {
				dup2(data, STDIN_FILENO);
				execvp(args[0], args);
			}

			if(data != 0) {
				close(data);
			}
			close(endp[1]);
			close(endp[0]);
		}

		if ((pid=waitpid(pid, &status, 0)) < 0)
			fprintf(stderr, "shell: waitpid error: %s\n",
					strerror(errno));
	}

	exit(EX_OK);
}
