#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READEND 0
#define WRITEEND 1

void child(int *p);

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        child(p);
    } else {
        close(p[READEND]);
        for (int i = 2; i < 35; i++) {
            write(p[WRITEEND], &i, sizeof(int));
        }
        close(p[WRITEEND]);
        wait(0);
    }
    exit(0);
}

void child(int *p) {
    int pr[2];
    int n;

    close(p[WRITEEND]);
    if (read(p[READEND], &n, sizeof(int)) == 0) {
        exit(0);
    }

    pipe(pr);

    if (fork() == 0) {
        child(pr);
    } else {
        close(pr[READEND]);
        printf("prime %d\n", n);
        int prime = n;
        while(read(p[READEND], &n, sizeof(int))) {
            if(n % prime != 0) {
                write(pr[WRITEEND], &n, sizeof(int));
            }
        }
        close(pr[WRITEEND]);
        wait(0);
    }

    exit(0);    

}