#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: cp src dst\n");
        exit(1);
    }

    int fd_src = open(argv[1], O_RDONLY);

    if (fd_src < 0)
    {
        printf("cp: cannot open %s\n", argv[1]);
        exit(2);
    }

    int fd_dst = open(argv[2], O_CREATE | O_WRONLY);

    if (fd_dst < 0)
    {
        printf("cp: cannot write to %s\n", argv[2]);
        close(fd_src);
        exit(3);
    }

    char buffer[512];
    int bytes_read = 0;
    while ((bytes_read = read(fd_src, buffer, sizeof(buffer))) > 0)
    {
        int bytes_written = write(fd_dst, buffer, bytes_read);
        if (bytes_written != bytes_read)
        {
            printf("cp: error while writing from src to dst\n");
            close(fd_src);
            close(fd_dst);
            exit(4);
        }
    }

    close(fd_src);
    close(fd_dst);

    exit(0);
}