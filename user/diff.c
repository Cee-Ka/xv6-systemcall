#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAXLINE 512

int
readline(int fd, char *buf, int max)
{
  int i = 0;
  char c;
  int n;

  if(max <= 0)
    return 0;

  while(i < max - 1){
    n = read(fd, &c, 1);
    if(n < 1)
      break;

    if(c == '\n')
      break;

    buf[i++] = c;
  }

  buf[i] = '\0';
  return i;
}

int
main(int argc, char *argv[])
{
  int fd1, fd2;
  int quiet = 0;
  char *file1, *file2;
  char buf1[MAXLINE], buf2[MAXLINE];
  int line = 1;

  if(argc < 3 || argc > 4){
    fprintf(2, "usage: diff file1 file2 [-q]\n");
    exit(1);
  }

  file1 = argv[1];
  file2 = argv[2];

  if(argc == 4){
    if(strcmp(argv[3], "-q") == 0){
      quiet = 1;
    } else {
      fprintf(2, "usage: diff file1 file2 [-q]\n");
      exit(1);
    }
  }

  fd1 = open(file1, 0);
  if(fd1 < 0){
    fprintf(2, "diff: cannot open %s\n", file1);
    exit(1);
  }

  fd2 = open(file2, 0);
  if(fd2 < 0){
    fprintf(2, "diff: cannot open %s\n", file2);
    close(fd1);
    exit(1);
  }

  while(1){
    int n1 = readline(fd1, buf1, MAXLINE);
    int n2 = readline(fd2, buf2, MAXLINE);

    int eof1 = (n1 == 0);
    int eof2 = (n2 == 0);

    if(eof1 && eof2)
      break;

    if(eof1)
      strcpy(buf1, "EOF");
    if(eof2)
      strcpy(buf2, "EOF");

    if(strcmp(buf1, buf2) != 0){
      if(quiet){
        printf("diff: files differ\n");
        close(fd1);
        close(fd2);
        exit(0);
      }

      printf("f1:%d: < %s\n", line, buf1);
      printf("f2:%d: > %s\n", line, buf2);
    }

    line++;
  }

  close(fd1);
  close(fd2);
  exit(0);
}