#include <stdio.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#define CPU_CORES 12

struct cpu_usage {
  long total;
  long used;
};

void printTemp(int hwmon_num) {
  long temp;
  char name[128];

  char path[128];

  sprintf(path, "/sys/class/hwmon/hwmon%d/temp1_input", hwmon_num);
  FILE* fp = fopen(path, "r");
  fscanf(fp, "%ld", &temp);
  fclose(fp);

  sprintf(path, "/sys/class/hwmon/hwmon%d/name", hwmon_num);
  fp = fopen(path, "r");
  fscanf(fp, "%s", &name);
  fclose(fp);

  printf("%s: %.2fC\n", name, temp / 1000.0);
}

int main() {
  long temp;
  char name[128];
  while (1) {
    printf("\nTemp Info\n");
    printf("--------\n");
    for (int i = 0; i < 4; i++) {
      printTemp(i);
    }
    sleep(1);
  };

  return 0;
}