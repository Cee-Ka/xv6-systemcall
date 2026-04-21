#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define MAX_TREE_PATH 512
#define MAX_TREE_DEPTH 2147483647

void print_usage()
{
  fprintf(2, "usage: tree [path] [-L depth] [-d]\n");
}

int is_dot_or_dotdot(char *name) // return 1 if . or .. else return 0
{
  return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

int is_dir(char *path)
{
  struct stat st;

  if (stat(path, &st) < 0)
    return -1;
  return st.type == T_DIR;
}

int join_path(char *out, int out_sz, char *dir, char *name)
{
  int dir_len;
  int name_len;
  int need_slash;

  dir_len = strlen(dir);
  name_len = strlen(name);
  need_slash = (dir_len > 0 && dir[dir_len - 1] != '/');

  if (dir_len + need_slash + name_len + 1 > out_sz)
    return -1;

  strcpy(out, dir);
  if (need_slash)
  {
    out[dir_len] = '/';
    out[dir_len + 1] = 0;
  }
  strcpy(out + strlen(out), name);
  return 0;
}

void print_indent(int level)
{
  int i;
  for (i = 0; i < level; ++i)
    printf("  ");
}

void tree_walk(char *path, int level, int max_depth, int only_dir)
{
  int fd;
  int dir_flag;
  struct dirent de;
  char name[DIRSIZ + 1];
  char child[MAX_TREE_PATH];

  if (level >= max_depth)
    return;

  fd = open(path, O_RDONLY);
  if (fd < 0)
  {
    fprintf(2, "tree: cannot open %s\n", path);
    return;
  }

  while (read(fd, &de, sizeof(de)) == sizeof(de))
  {
    if (de.inum == 0)
      continue;

    memmove(name, de.name, DIRSIZ);
    name[DIRSIZ] = 0;
    if (is_dot_or_dotdot(name))
      continue;

    if (join_path(child, sizeof(child), path, name) < 0)
    {
      fprintf(2, "tree: path too long: %s/%s\n", path, name);
      continue;
    }

    dir_flag = is_dir(child);
    if (dir_flag < 0)
    {
      fprintf(2, "tree: cannot stat %s\n", child);
      continue;
    }

    if (only_dir && !dir_flag)
      continue;

    print_indent(level + 1);
    printf("|-- %s\n", name);

    if (dir_flag)
      tree_walk(child, level + 1, max_depth, only_dir);
  }

  close(fd);
}

int main(int argc, char *argv[])
{
  int dir_flag;
  int only_dir = 0;
  int max_depth = MAX_TREE_DEPTH;
  int has_path = 0;
  char *path;
  path = ".";

  for (int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "-d") == 0)
    {
      only_dir = 1;
      continue;
    }

    if (strcmp(argv[i], "-L") == 0)
    {
      if (i + 1 >= argc)
      {
        print_usage();
        exit(1);
      }
      max_depth = atoi(argv[i + 1]);
      if (max_depth < 0)
      {
        print_usage();
        exit(1);
      }
      ++i;
      continue;
    }

    if (argv[i][0] == '-')
    {
      print_usage();
      exit(1);
    }

    if (has_path)
    {
      print_usage();
      exit(1);
    }
    path = argv[i];
    has_path = 1;
  }

  dir_flag = is_dir(path);
  if (dir_flag < 0)
  {
    fprintf(2, "tree: cannot stat %s\n", path);
    exit(1);
  }

  if (!dir_flag)
  {
    if (!only_dir)
      printf("%s\n", path);
    exit(0);
  }

  printf("%s\n", path);
  tree_walk(path, 0, max_depth, only_dir);
  exit(0);
}
