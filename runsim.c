#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_NUM 1000
#define BUF_SIZE 1000

int main(int argc, char *argv[]) {
    // if (argc != 2) {
    if (argc == 1) {
        fprintf(stderr, "Error: Only 1 argument. \n");
        exit(EXIT_FAILURE);
    }

    int pr_limit;
    int pr_count = 0;
    char* buffer = (char*) malloc(BUF_SIZE);
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    int status;
    int arg;
    
    sscanf(argv[1], "%d", &arg);
    if (arg >= 1 && arg <= MAX_NUM) {
        sscanf(argv[1], "%d", &pr_limit);
    } 
    else {
        fprintf(stderr, "Error: pr_limit should be an integer between 1 and %d.\n", MAX_NUM);
        exit(EXIT_FAILURE);
    }

    // up to the size of (BUF_SIZE - 1).
    while (fgets(buffer, BUF_SIZE - 1, stdin) != NULL) {
        if (strlen(buffer) == BUF_SIZE - 1 && buffer[BUF_SIZE - 2] != '\n') {
            fprintf(stderr, "Error: line too long.\n");
            break;
        }
        if (strlen(buffer) == 0 && buffer[0] == '\n') {
            continue;
        }
        if (pr_count == pr_limit) {
            pid = wait(&status);
            if (pid == -1) {
                perror("wait");
                exit(EXIT_FAILURE);
            }
            printf("The process that returned is = %d", pid);
            if (WIFEXITED (status))
                printf ("Normal termination with exit status = %d\n", WEXITSTATUS (status));
            if (WIFSIGNALED (status))
                printf ("Killed by signal = %d%s\n", WTERMSIG (status), WCOREDUMP (status) ? " (dumped core)" : "");
            if (WIFSTOPPED (status))
                printf ("Stopped by signal = %d\n", WSTOPSIG (status));
            if (WIFCONTINUED (status))
                printf ("Continued\n");
            pr_count--;
        }
        
        pid = fork();
        pr_count++;
        if (pid == -1) {
            perror("fork");
            pr_count--;
        }
        else if (pid == 0) {
            char * arg[4];
            arg[0] = "bash";
            arg[1] = "-c";
            arg[2] = buffer;
            arg[3] = NULL;
            execv("/bin/bash", arg);
            perror("execv");
            exit(EXIT_FAILURE);
        }
        else {
            printf("pid: %d is successfully forked\n", pid);
            pid = waitpid(-1, &status, WNOHANG); 
            // Thereby, you will check if any of the children have returned in the meantime.
            while (pid != 0) {
                // pid = wait(&status);
                if (pid == -1) {
                    perror("waitpid");
                }
                else {
                    printf("The process that returned is = %d", pid);
                    if (WIFEXITED (status))
                        printf ("Normal termination with exit status = %d\n", WEXITSTATUS (status));
                    if (WIFSIGNALED (status))
                        printf ("Killed by signal = %d%s\n", WTERMSIG (status), WCOREDUMP (status) ? " (dumped core)" : "");
                    if (WIFSTOPPED (status))
                        printf ("Stopped by signal = %d\n", WSTOPSIG (status));
                    if (WIFCONTINUED (status))
                        printf ("Continued\n");
                    pr_count--;
                    
                }
                pid = waitpid(-1, &status, WNOHANG); 
            }
        }
    }
    // We will now wait for all the remaining children to return:
    while (pr_count > 0) {
        pid = wait(&status); 
        if (pid == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        printf("The process that returned is = %d", pid);
        if (WIFEXITED (status))
            printf ("Normal termination with exit status = %d\n", WEXITSTATUS (status));
        if (WIFSIGNALED (status))
            printf ("Killed by signal = %d%s\n", WTERMSIG (status), WCOREDUMP (status) ? " (dumped core)" : "");
        if (WIFSTOPPED (status))
            printf ("Stopped by signal = %d\n", WSTOPSIG (status));
        if (WIFCONTINUED (status))
            printf ("Continued\n");
        pr_count --;
    }

    free(buffer);
    return EXIT_SUCCESS;
}
