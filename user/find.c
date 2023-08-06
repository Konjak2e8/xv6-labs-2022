#include <kernel/types.h>
#include <kernel/stat.h>
#include <kernel/fs.h>
#include <user/user.h>

#define BUF_SIZE 512

void strMatch(char const *str1, char const *str2) { // 字符串匹配
    int l1 = strlen(str1), l2 = strlen(str2);
    for (int p1 = 0, p2 = 0; p1 < l1; p1++) {
        if (str1[p1] == str2[p2]) {
            while (str1[p1++] == str2[p2++] && p1 < l1 && p2 < l2)
                ;
            if (p2 == l2) {
                printf("%s\n", str1);
                return;
            }
        }
    }
}

void find(char const *path, char const *name) {
    char buf[BUF_SIZE], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0) { // 打开路径下的文件，如不存在则输出错误信息
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }
    
    if(fstat(fd, &st) < 0) { // 显示文件信息，如无法显示则输出错误信息
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch(st.type){
        case T_FILE: // 对于文件名，直接匹配
            strMatch(path, name);
            break;

        case T_DIR: // 对于目录名，递归处理目录下的文件和子目录
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                printf("ls: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if(de.inum == 0)
                    continue;
                if(de.name[0] == '.' && de.name[1] == 0) continue;
                if(de.name[0] == '.' && de.name[1] == '.' && de.name[2] == 0) continue;
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0){
                    printf("ls: cannot stat %s\n", buf);
                    continue;
                }
                find(buf, name); // 递归查找
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]){
    if (argc < 3) { // 判断输入错误
        printf("Usage: find [path] [filename]\n");
        exit(-1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
