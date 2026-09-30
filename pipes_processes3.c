#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <grep argument>\n", argv[0]);
        return 1;
    }

    int fd1[2];  // cat  -> grep
    int fd2[2];  // grep -> sort

    if (pipe(fd1) == -1 || pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed\n");
        return 1;
    }

    pid_t p = fork();
    if (p < 0)
    {
        fprintf(stderr, "fork Failed\n");
        return 1;
    }

    if (p > 0)
    {
        // P1 (parent): cat scores
        dup2(fd1[1], STDOUT_FILENO);   // stdout -> fd1 write end
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);

        execlp("cat", "cat", "scores", NULL);
        perror("execlp cat");
        exit(1);
    }
    else
    {
        pid_t p2 = fork();
        if (p2 < 0)
        {
            fprintf(stderr, "fork Failed\n");
            return 1;
        }

        if (p2 > 0)
        {
            // P2 (child): grep <argv[1]>
            dup2(fd1[0], STDIN_FILENO);    // stdin  <- fd1 read end
            dup2(fd2[1], STDOUT_FILENO);   // stdout -> fd2 write end
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);

            execlp("grep", "grep", argv[1], NULL);
            perror("execlp grep");
            exit(1);
        }
        else
        {
            // P3 (child's child): sort
            dup2(fd2[0], STDIN_FILENO);    // stdin <- fd2 read end
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);

            execlp("sort", "sort", NULL);
            perror("execlp sort");
            exit(1);
        }
    }

    return 0;
}
