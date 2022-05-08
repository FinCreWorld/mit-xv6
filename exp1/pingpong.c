// 实现 pingpong 程序
// 父进程将 ping 写入管道
// 子进程读出 ping，并打印
// 子进程将 pong 写入管道
// 父进程读出 pong 并打印
#include "kernel/types.h"
#include "user/user.h"
const int MAXN = 105;
int pipe_read(int p[2], char *buffer, int n) {
    close(p[1]);
    int r_len = read(p[0], buffer, n);
    close(p[0]);
    return r_len;
}
int pipe_write(int p[2], char *buffer, int n) {
    close(p[0]);
    int w_len = write(p[1], buffer, n);
    close(p[1]);
    return w_len;
}
int main(int argc, char *argv[]) {
    int p1[2];  // 从父进程向子进程写入的管道
    int p2[2];  // 从子进程向父进程写入的管道
    char buffer[MAXN];
    if (pipe(p1) || pipe(p2)) {
        printf("Failed to create pipe!\n");
        exit(-1);
    }
    if (fork() == 0) {
        /* son */
        /* read from pipe1 */
        pipe_read(p1, buffer, MAXN);
        printf("%d: %s\n", getpid(), buffer);
        /* write to pipe2 */
        pipe_write(p2, "received pong", MAXN);
    } else {
        /* parent */
        /* write to pipe1 */
        pipe_write(p1, "received ping", MAXN);
        /* read from pip2 */
        pipe_read(p2, buffer, MAXN);
        printf("%d: %s\n", getpid(), buffer);
        int status;
        wait(&status);
    }
    exit(0);
}