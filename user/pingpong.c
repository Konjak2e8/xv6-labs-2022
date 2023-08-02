#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>
int main(int argv, char **argc) {
    char buf[10];
    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);
    if (fork() == 0) {
        if (read(p1[0], buf, 1)) {
            fprintf(2, "%d: received ping\n", getpid());
            write(p2[1], "p", 1);
            exit(0);
        }
    }
    else {
        write(p1[1], buf, 1);
        if (read(p2[0], buf, 1)) {
            fprintf(1, "%d: received pong\n", getpid());
        }
    }
    exit(0);
}