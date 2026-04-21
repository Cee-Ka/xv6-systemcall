#include "kernel/types.h" 
#include "user/user.h" 

int main(){
    int fd1[2]; // fd 3 read , fd 4 write cha -> con
    int fd2[2]; // fd 5 read , fd 6 write con -> cha
    char buffer[1];
    pipe(fd1);
    pipe(fd2);
    if(fork() == 0) {
        close(fd1[1]);
        read(fd1[0], buffer, 1);
        if(buffer[0] == '1')
            printf("%d: received ping\n", getpid());
        close(fd1[0]);
        close(fd2[0]);
        write(fd2[1], "1", 1);
        close(fd2[1]);
    } else {
        close(fd1[0]);
        write(fd1[1], "1", 1);
        close(fd1[1]);
        close(fd2[1]);
        read(fd2[0], buffer, 1);
        if(buffer[0] == '1')
            printf("%d: received ping\n", getpid());
        close(fd2[0]);
    }
    exit(0);
}