#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// get the file name of a file path
char* get_name(char *path) {
    char *p;
    for (p=path+strlen(path); p >= path && *p != '/'; --p)
    ;
    ++p;
    return p;
}

void find(char *path, char *fn) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {  // get file descriptor of path
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    
    if (fstat(fd, &st) < 0) {       // get file stat
        fprintf(2, "find: cannot stat %s\n", path);
        return;
    }

    if (strcmp(fn, get_name(path)) == 0)
        printf("%s\n", path);

    if (st.type == T_DIR) {
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';                 // add slash to the end of buf
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) continue; 
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);    // fill the path
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            find(buf, fn);
        }
    }
    close(fd);
    return;
}

int main(int argc, char *argv[]) {
// int main() {
    // int argc = 3;
    // char *argv[] = {".", "b"};
    if (argc < 3)
        printf("missing argments\n");
    else {
        char *path = argv[1];
        char *fn  = argv[2];    // file name

        char *p = path + strlen(path);  // remove the last slash
        if (*--p == '/') *p = 0;

        find(path, fn);
    }
    exit(0);
}