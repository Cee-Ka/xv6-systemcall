#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/ptree.h"
#include "user/user.h"

static struct ptreeinfo procs[NPROC];
static int nprocs;

static void
print_tree(int pid, int depth)
{
  for(int i = 0; i < nprocs; i++){
    if(procs[i].pid == pid){
      for(int k = 0; k < depth; k++)
        printf(" ");

      printf("%d %s state=%d mem=%d\n",
             procs[i].pid,
             procs[i].name,
             procs[i].state,
             (int)procs[i].memsize);

      for(int j = 0; j < nprocs; j++){
        if(procs[j].ppid == pid){
          print_tree(procs[j].pid, depth + 1);
        }
      }
      return;
    }
  }
}

int
main(void)
{
  nprocs = ptree(procs, NPROC);
  if(nprocs < 0){
    printf("pstree: ptree failed\n");
    exit(1);
  }

  for(int i = 0; i < nprocs; i++){
    int parent_found = 0;
    for(int j = 0; j < nprocs; j++){
      if(procs[i].ppid == procs[j].pid){
        parent_found = 1;
        break;
      }
    }

    if(!parent_found){
      print_tree(procs[i].pid, 0);
    }
  }

  exit(0);
}