#include <kernel/types.h>
#include <kernel/stat.h>
#include <kernel/fs.h>
#include <user/user.h>

#define BUF_SIZE 512

void strMatch(char const *str1, char const *str2) {
    int p1 = 0, p2 = 0, l1 = strlen(str1), l2 = strlen(str2);
    for (; p1 < l1; p1++) {
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

    if((fd = open(path, 0)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }
    
    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch(st.type){
        case T_FILE:
            strMatch(path, name);
            break;

        case T_DIR:
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
                find(buf, name);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]){
    if (argc < 3){
        printf("Usage: find [path] [filename]\n");
        exit(-1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
