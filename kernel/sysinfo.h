#include "kernel/types.h"

struct sysinfo
{
    uint64 freemem; // số byte RAM trống
    uint64 nproc;   // số process có trạng thái khác UNUSED
};
