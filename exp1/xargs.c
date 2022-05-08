#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

const int MAXARGLEN = 64;
const int CHARSIZE = sizeof(char);

int readline(char *nargv[], int *nargc) {
    int over;
    int i = 0;
    char ch;
    *nargc = 0;

    // read the space before the input
    while((over = read(0, &ch, CHARSIZE)) && (ch == ' ' || ch == '\t'));
    if (!over) {
        nargv[*nargc] = 0;
        return over;
    }

    while((ch != '\n') && over) {
        nargv[*nargc] = (char*) malloc(MAXARGLEN * CHARSIZE);
        nargv[*nargc][i++] = ch;
        // read a continuous word without space
        while((over = read(0, &ch, CHARSIZE)) && (ch != ' ') && (ch != '\t') && (ch != '\n'))
            nargv[*nargc][i++] = ch;
        nargv[(*nargc)++][i] = '\0';
        i = 0;
        if (ch == '\n') break;
        // read the space after the word
        while(over && (over = read(0, &ch, CHARSIZE)) && ((ch == ' ') || (ch == '\t')));
    }
    nargv[*nargc] = 0;
    return over;
}

void release(int nargc, char *nargv[]) {
    for (int i = 0; i < nargc; ++i)
        free(nargv[i]);
}

//find . README | xargs echo hi

int main(int argc, char *argv[]) {
    char *args[MAXARG];
    for (int i = 0; i < argc; ++i)
        args[i] = argv[i];
    if (argc > MAXARG) {
        printf("Too many args\n");
        exit(0);
    }

    int over = 1;
    int nargc;
    while(over) {
        over = readline(args + argc, &nargc);
        if (!over || !nargc) break;

        int pid = fork();
        if (pid == 0) {
            exec(args[1], args + 1);
        } else if (pid < 0) {
            printf("failed to create child process!\n");
            exit(-1);
        }

        release(nargc, args + argc);
        int status;
        wait(&status);
    }

    exit(0);
}