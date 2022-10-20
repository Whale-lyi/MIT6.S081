#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

void find(char *path, char* file);

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(2, "find: You need pass in only 2 argument");
        exit(1);
    }

    char *target_path = argv[1];
    char *target_file = argv[2];
    find(target_path, target_file);
    exit(0);
}

void find(char *path, char* file) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
        case T_FILE:
            if (strcmp(path + strlen(path) - strlen(file), file) == 0) {
                printf("%s\n", path);
            }
            break;
        case T_DIR:
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);  
            p = buf + strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                // 获取完整路径名
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0){
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                // 递归时不要进入.和..
                if ((strcmp(de.name, ".") != 0) && (strcmp(de.name, "..") != 0)) {
                    find(buf, file);
                }
            }
            break;
    }
    close(fd);
}