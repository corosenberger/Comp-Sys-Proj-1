#include "Problem3.h"

int main(int argc, char const *argv[]) {
    char exitCode = 'A';
    char killParent = '\0';
    int sleepTime = 1000000;

    if(argc > 1) {
        killParent = *argv[1];
        if(argc > 2) sleepTime *= atof(argv[2]);
    }

    int id1, id2;

    printf("Creating child from parent: %i\n",getpid());
    if(id1=fork()) {
        printf("Parent: %i successfully created child: %i\n", getpid(), id1);
    } else {
        printf("Child: %i successfully created with parent: %i\n", getpid(), getppid());
        exitCode++;
    }

    printf("Creating child from parent: %i\n", getpid());
    if(id2=fork()) {
        printf("Parent: %i successfully created child: %i\n", getpid(), id2);
    } else {
        printf("Child: %i successfully created with parent: %i\n", getpid(), getppid());
        exitCode++; exitCode++;
    }
    
    if(killParent == exitCode) {
        printf("Killing process: %i with signal SIGINT\n", getpid());
        raise(SIGINT);
    } else {
        printf("Putting process: %i with parent: %i to sleep for %iμs\n", getpid(), getppid(), sleepTime);
        usleep(sleepTime);
    }

    if(id1) waitAndPrint(id1);
    if(id2) waitAndPrint(id2);

    printf("Process: %i with parent: %i exiting with exit code: %c\n", getpid(), getppid(), exitCode);
    return exitCode;
}

void waitAndPrint(int id) {
    int status;
    printf("Attempting to reap child: %i with its parent: %i\n", id, getpid());
    if(waitpid(id, &status, 0) != -1) {
        printf("Child: %i successfully reaped by parent: %i\n", id, getpid());
        if(WIFEXITED(status)) {
            printf("Child: %i exited naturally with exit status: %c\n", id, WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)) {
            printf("Child: %i was terminated with signal: %i\n", id, WTERMSIG(status));
        }
    } else {
        printf("Parent: %i failed to reap child: %i\n", getpid(), id);
    }
}