#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#define BUF_SIZE 256

int main()
{
    int fd1[2];  // P1 -> P2
    int fd2[2];  // P2 -> P1

    char fixed_str[]  = "howard.edu";
    char fixed_str2[] = "gobison.org";
    char input_str[BUF_SIZE];
    pid_t p;

    if (pipe(fd1) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }
    if (pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    printf("Other string is: %s\n", fixed_str);
    printf("Input : ");
    fflush(stdout);
    if (scanf("%99s", input_str) != 1)
    {
        fprintf(stderr, "No input\n");
        return 1;
    }

    fflush(stdout);

    p = fork();

    if (p < 0)
    {
        fprintf(stderr, "fork Failed");
        return 1;
    }
    else if (p > 0)
    {
        // Parent (P1)
        char concat_str[BUF_SIZE];

        close(fd1[0]);
        close(fd2[1]);

        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]);

        read(fd2[0], concat_str, BUF_SIZE);
        close(fd2[0]);

        wait(NULL);

        strncat(concat_str, fixed_str2, BUF_SIZE - strlen(concat_str) - 1);
        printf("Output : %s\n", concat_str);
    }
    else
    {
        // Child (P2)
        char concat_str[BUF_SIZE];
        char second_str[BUF_SIZE];

        close(fd1[1]);
        close(fd2[0]);

        read(fd1[0], concat_str, BUF_SIZE);
        close(fd1[0]);

        strncat(concat_str, fixed_str, BUF_SIZE - strlen(concat_str) - 1);
        printf("Output : %s\n", concat_str);

        printf("Input : ");
        fflush(stdout);
        if (scanf("%99s", second_str) != 1)
            second_str[0] = '\0';
        strncat(concat_str, second_str, BUF_SIZE - strlen(concat_str) - 1);

        fflush(stdout);

        write(fd2[1], concat_str, strlen(concat_str) + 1);
        close(fd2[1]);

        exit(0);
    }

    return 0;
}
