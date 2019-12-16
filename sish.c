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

static int run(char* cmd, int input, int first, int last);
static char line[1024];
static int n = 0; /* number of calls to 'command' */
static void split(char* cmd);

static char* args[512];
pid_t pid;
char buf[1024];
char* query = NULL;
char* buf_cmd;
char ch;

int status = 0;
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

int main(int argc, char** argv)
{
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
                                        default:
                                                sish_help();
                                break;
        }
    }

        while (1) {
                printf("sish$ ");
                fflush(NULL);

                if (!fgets(line, 1024, stdin))
                        return 0;

                int input = 0;
                int first = 1;

                char* cmd = line;
                printf("line = %s", line);
                if(strcmp("echo $$\n", line) == 0) {
                        printf("%d\n", (int)getpid());
                        status = 0;
                        continue;
                } else if(strcmp("echo $?\n", line) == 0) {
                        printf("%d\n", status);
                        status = 0;
                        continue;
                } else {
                        char* next = strchr(cmd, '|');

                        while (next != NULL) {
                                *next = '\0';
                                input = run(cmd, input, first, 0);

                                cmd = next + 1;
                                next = strchr(cmd, '|');
                                first = 0;
                        }
                        input = run(cmd, input, first, 1);
                        for (int i = 0; i < n; ++i)
                                wait(NULL);
                }
                n = 0;
        }
        return 0;
}

static int run(char* cmd, int input, int first, int last)
{
        split(cmd);
        if (args[0] != NULL) {
                if (strcmp(args[0], "exit") == 0)
                        exit(0);
                n += 1;
                return command(input, first, last);
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


