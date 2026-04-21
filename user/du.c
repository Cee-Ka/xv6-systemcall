#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "user/user.h"

#define MAXPATH 512

long
du(char *path, int print_all, int summary, int is_root)
{
  int fd;
  struct stat st;
  struct dirent de;
  char buf[MAXPATH];
  long total = 0;

  fd = open(path, O_RDONLY);
  if(fd < 0){
    fprintf(2, "du: cannot open %s\n", path);
    return 0;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "du: cannot stat %s\n", path);
    close(fd);
    return 0;
  }

  if(st.type == T_FILE){
    total = st.size;
    if(print_all && !summary)
      printf("%ld\t%s\n", total, path);
    close(fd);
    return total;
  }

  if(st.type != T_DIR){
    close(fd);
    return 0;
  }

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    struct stat st2;
    int childfd;
    int len;

    if(de.inum == 0)
      continue;

    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    len = strlen(path);
    if(len + 1 + DIRSIZ + 1 > sizeof(buf)){
      fprintf(2, "du: path too long\n");
      continue;
    }

    strcpy(buf, path);
    buf[len] = '/';
    memmove(buf + len + 1, de.name, DIRSIZ);
    buf[len + 1 + DIRSIZ] = 0;

    childfd = open(buf, O_RDONLY);
    if(childfd < 0){
      fprintf(2, "du: cannot open %s\n", buf);
      continue;
    }

    if(fstat(childfd, &st2) < 0){
      fprintf(2, "du: cannot stat %s\n", buf);
      close(childfd);
      continue;
    }
    close(childfd);

    if(st2.type == T_FILE){
      total += st2.size;
      if(print_all && !summary)
        printf("%ld\t%s\n", (long)st2.size, buf);
    } else if(st2.type == T_DIR){
      total += du(buf, print_all, summary, 0);
    }
  }

  close(fd);

  if(summary){
    if(is_root)
      printf("%ld\t%s\n", total, path);
  } else {
    printf("%ld\t%s\n", total, path);
  }

  return total;
}

int
main(int argc, char *argv[])
{
  char *path = ".";
  int print_all = 0;
  int summary = 0;
  int i;

  for(i = 1; i < argc; i++){
    if(strcmp(argv[i], "-a") == 0)
      print_all = 1;
    else if(strcmp(argv[i], "-s") == 0)
      summary = 1;
    else
      path = argv[i];
  }

  du(path, print_all, summary, 1);
  exit(0);
}