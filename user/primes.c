#include "kernel/types.h" 
#include "user/user.h" 
#include "stdbool.h"
int main(){
    int fd[2];
    pipe(fd);
    if(fork() == 0){
        int mul;
        int p;
        close(fd[1]);
        read(fd[0], &p, 4);
        printf("prime %d\n", p);
        mul = p;
        int p1; // dau read
        p1 = fd[0];
        while(true){
            int fd1[2];
            pipe(fd1);
            if(fork() == 0){
                close(fd1[1]);
                int n = read(fd1[0], &p, sizeof(p));
                if(n != sizeof(p)) exit(0);
                printf("prime %d\n", p);
                p1 = fd1[0];
                p1++;
                p1--;
                mul = p;    
            }
            else{
                close(fd1[0]);
                while(read(p1, &p, 4) > 0)
                {
                    if(p % mul)
                        write(fd1[1], &p, 4);

                }
                close(fd1[1]);
                close(p1);
                wait((int *) 0);
                exit(0);
            }
        }
        
    }
    else{
        close(fd[0]);
        for(int i = 2; i <= 280; i++)
            write(fd[1], &i, 4);
        close(fd[1]);
        wait((int *) 0);
    }
    exit(0);
}
