#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;
  char *nargv[MAXARG];

  // Kiểm tra xem người dùng có nhập đủ tham số hay không
  // Cú pháp đúng phải là: trace <mask> <command> [args...]
  if(argc < 3 || (argv[1][0] < '0' || argv[1][0] > '9')){
    fprintf(2, "Usage: %s mask command\n", argv[0]);
    exit(1);
  }

  // Gọi system call trace vừa viết trong kernel với tham số mask
  if (trace(atoi(argv[1])) < 0) {
    fprintf(2, "%s: trace failed\n", argv[0]);
    exit(1);
  }

  // Lấy các đối số còn lại (từ argv[2] trở đi) để đưa vào hàm exec
  for(i = 2; i < argc && i < MAXARG; i++){
    nargv[i-2] = argv[i];
  }
  nargv[i-2] = 0; // Kết thúc mảng tham số bằng null

  // Thực thi chương trình mới (ví dụ: grep)
  exec(nargv[0], nargv);
  
  // Nếu exec thành công, nó sẽ không bao giờ chạy tới dòng này
  // Nếu chạy tới đây nghĩa là exec thất bại
  fprintf(2, "exec %s failed\n", nargv[0]);
  exit(1);
}