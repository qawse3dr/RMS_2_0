
#include <sys/sysinfo.h>

#include <stdio.h>
int main() {

    struct sysinfo info;

    int rtn = sysinfo(&info);
    printf("uptime minutes: %f\n", info.uptime/60.0 );
    return 0;
}


