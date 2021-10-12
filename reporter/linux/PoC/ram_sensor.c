#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>
int main() {
  long total_mem, free_mem;
  long swap_mem, free_swap_mem;

  while (1) {
    FILE* fp = fopen("/proc/meminfo", "r");
    fscanf(fp, "MemTotal: %ld kB\n", &total_mem);
    fscanf(
        fp,
        "MemFree: %*ld kB\n");  // Ignore line This doesn't give correct value.
    fscanf(fp, "MemAvailable: %ld kB\n", &free_mem);

    for (int i = 0; i < 11; i++) {
      while ((char)fgetc(fp) != '\n')
        ;
    }
    fscanf(fp, "SwapTotal: %ld kB\n", &swap_mem);
    fscanf(fp, "SwapFree: %ld kB\n",
           &free_swap_mem);  // Ignore line This doesn't give correct value.

    printf("\nMem Usage\n");
    printf("---------\n");

    printf("Mem Usage: %f/%fGB\n", (total_mem - free_mem) / (1024.0 * 1024.0),
           total_mem / (1024.0 * 1024.0));
    printf("Swap Usage: %f/%fGB\n",
           (swap_mem - free_swap_mem) / (1024.0 * 1024.0),
           swap_mem / (1024.0 * 1024.0));
    fclose(fp);
    sleep(1);
  }
}
