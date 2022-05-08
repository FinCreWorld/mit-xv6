#include "kernel/types.h"
#include "user/user.h"

const int N = 10;
const int INT_SIZE = 4;

void open_pipe(int p[2]) {
    if (pipe(p)) {
        printf("failed to create pipe!\n");
        exit(-1);
    }
}

void child_process(int p0, int p1) {
    close(p1);    // shutdown the write port from parent process
    int prime;      // the first number from parent process
    int num;        // data from parent process except prime
    int has_child = 0;  // if has a child

    int r_len = read(p0, &prime, INT_SIZE);
    if (r_len <= 0) exit(0);
    printf("prime %d\n", prime);
    
    int p2[2];      // pipe from self to son process
    while(read(p0, &num, INT_SIZE)) {
        if (!(num % prime)) continue;
        if (!has_child) {
            has_child = 1;
            open_pipe(p2);
            // shutdown the read port
            int p20 = p2[0], p21 = p2[1];
            int pid = fork();
            if (!pid) {
                child_process(p20, p21);
            } else if (pid < 0) {
                printf("failed to create child process!\n");
                exit(-2);
            }
            close(p2[0]);
        }
        int w_len = write(p2[1], &num, INT_SIZE);
        if (w_len < 0) {
            printf("failed to write pipe\n");
            exit(-3);
        }
    }
    close(p0);    // shutdown the read port from parent process
    close(p2[1]);   // shutdown the write port to child process
    int status;
    wait(&status);
    exit(0);
}

void root_process() {
    int p[2];
    open_pipe(p);
    int p0 = p[0], p1 = p[1];
    if (!fork()) {
        child_process(p0, p1);
    } else {
        // parent process
        close(p[0]);    // shutdown the read port
        for (int i = 2; i <= 35; ++i) {
            int w_len = write(p[1], &i, INT_SIZE);
            if (w_len <= 0) {
                printf("failed to write pipe\n");
                exit(-3);
            }
        }
        close(p[1]);
        int status;
        wait(&status);
    }
    return;
}

int main() {
    root_process();
    exit(0);
}