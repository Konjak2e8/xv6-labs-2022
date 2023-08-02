#include "kernel/types.h"
#include "user/user.h"

void pipline(int rp) { 
    // 从上一个过滤器读取一个数
    int p, n;
    // 读取失败，说明已经没有数可以处理了
    if (0 == read(rp, &p, sizeof(int))) {
        close(rp);
        exit(0);
    }

    printf("prime %d\n", p);
    
    // 创建新管道，连接当前进程和要创建的子进程
    int ppl[2];
    pipe(ppl);

    if (fork() == 0) {   // 创建子进程
        close(ppl[1]);   // 关闭写端
        pipline(ppl[0]); // 将父进程创建的管道的读端作为参数传给子进程进行读取
    } else {
        close(ppl[0]); // 关闭读端
        for (;;) {
            // 从上一个过滤器读取一个数
            if (0 == read(rp, &n, sizeof(int))) {
                // 已经读完上一个过滤器产生的数
                close(rp);
                break;
            }
            // 如果不能整除，发送到下一个过滤器
            if (n % p != 0) {
                write(ppl[1], &n, sizeof(int));
            }
        }
        close(ppl[1]);
        wait(0); // 等待子进程返回
        exit(0); // 进程退出
    }
}

int main(int argc, char const *argv[]) {
    int p[2];
    pipe(p);

    if (fork() == 0) {  // 开始pipline
        close(p[1]);    // 关闭写端
        pipline(p[0]);  // 将父进程创建的管道的读端作为参数传给子进程进行读取
        exit(0);
    } else {            // 主进程生成数字2-35
        close(p[0]);    // 关闭读端
        int i;
        for (i = 2; i <= 35; ++i) // 向子进程写入2-35的数字
            write(p[1], &i, sizeof(int));
        close(p[1]);
        wait(0);  // 等待子进程返回
        exit(0);
    }
}