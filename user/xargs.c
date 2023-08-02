#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    // 将xargs的参数保存在args中
    char *args[MAXARG];
    int i;
    for (i = 0; i < argc; ++i)
        args[i] = argv[i];
    
    char buf[256];

    for (;;) {
        // 从标准输入读取一行输入
        int j = 0;
        while ((0 != read(0, buf + j, sizeof(char))) && buf[j] != '\n')
            ++j;
        if (j == 0) break; // 读完所有行
        buf[j] = 0; // 字符串结尾
        // 将标准输入传进的一行参数附加到args中
        args[i] = buf;
        args[i+1] = 0;
        // 执行命令
        if (fork() == 0) {
            exec(args[1], args + 1);
            printf("exec error\n");
        } 
        else {
            wait(0); // 等待子进程执行完毕
        }
    }
    
    exit(0);
}