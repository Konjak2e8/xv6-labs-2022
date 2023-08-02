#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

#define NUM_SUBPROCESSES 5

int main(int argc, char **argv) {
    int pid, status;
    int i;
    int child_count = 0; // 子进程计数器

    for (i = 0; i < NUM_SUBPROCESSES; i++) {
        pid = fork();
        if (pid == 0) {
            break;  // 子进程跳出循环
        }
        child_count++;
    }

    if (pid == 0) {
        char *args[] = {"echo", "this", "is", "echo", 0};
        exec("echo", args);
        printf("exec failed!\n");
        exit(1);
    } else {
        while (child_count > 0) {
            int child_pid = wait(&status);
            printf("child process %d exited (child #%d) with status %d\n", child_pid, child_count, status);
            child_count--;
        }
    }
    exit(0);
}
