#include <kernel/defs.h>
#include <user/user.h>
char buf;
int main(int argv, char **argc) {
    int p[2];
    pipe(p);
    int son_pid = fork();
    if (pid > 0) {
        close(p[0]);
        if (write(p[1], buf, sizeof(buf)) > 0)
    }
}