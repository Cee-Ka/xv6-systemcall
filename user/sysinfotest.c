#include "kernel/types.h"
#include "kernel/sysinfo.h"
#include "user/user.h"

void fail(char *msg)
{
    printf("sysinfotest: %s\n", msg);
    exit(1);
}

void testcall(void)
{
    struct sysinfo info;

    if (sysinfo(&info) < 0)
        fail("sysinfo failed");
}

void testmem(void)
{
    struct sysinfo before, after;

    if (sysinfo(&before) < 0)
        fail("sysinfo failed");

    if (sbrk(4096) == (char *)-1)
        fail("sbrk failed");

    if (sysinfo(&after) < 0)
        fail("sysinfo failed");

    if (after.freemem >= before.freemem)
        fail("freemem not decreased");
}

void testproc(void)
{
    struct sysinfo before, after;
    int pid;

    if (sysinfo(&before) < 0)
        fail("sysinfo failed");

    pid = fork();
    if (pid < 0)
        fail("fork failed");

    if (pid == 0)
    {
        sleep(50);
        exit(0);
    }

    if (sysinfo(&after) < 0)
        fail("sysinfo failed");

    if (after.nproc <= before.nproc)
        fail("nproc not increased");

    wait(0);
}

int main(void)
{
    testcall();
    testmem();
    testproc();
    printf("sysinfotest: OK\n");
    exit(0);
}
