#include <stdio.h>

#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/resource.h>

#define CPU_CORES 12

struct cpu_usage{
	long total;
	long used;
};

void getCpuUsage(struct cpu_usage* cpu_usage, struct cpu_usage cpu_core_usage[CPU_CORES]) {
	long user, nice, system, idle, iowait, irq, softirq;
	int cpu_num;

	FILE* fp = fopen("/proc/stat", "r");
	
	fscanf(fp, "cpu %ld %ld %ld %ld %ld %ld %ld %*ld %*ld %*ld\n", &user, &nice, &system, &idle, &iowait, &irq, &softirq);
	cpu_usage->total = user + nice + system + idle + iowait + irq + softirq;
	cpu_usage->used = user + nice + system;

	while(fscanf(fp, "cpu%d %ld %ld %ld %ld %ld %ld %ld %*ld %*ld %*ld\n", &cpu_num, &user, &nice, &system, &idle, &iowait, &irq, &softirq)){
		cpu_core_usage[cpu_num].total = user + nice + system + idle + iowait + irq + softirq;
		cpu_core_usage[cpu_num].used = user + nice + system;

	}

	fclose(fp);
}

int main() {
	
	struct cpu_usage master_cpu_usage;
	struct cpu_usage master_cpu_usage_old;
	
	struct cpu_usage cpu_core_usage[CPU_CORES];
	struct cpu_usage cpu_core_usage_old[CPU_CORES];

	getCpuUsage(&master_cpu_usage_old,cpu_core_usage_old);

	sleep(1);
	while(1) {
		printf("\nCpu Info\n");
		printf("--------\n");

		getCpuUsage(&master_cpu_usage, cpu_core_usage);

		printf("master cpu usage: %.2f%%\n", 100.0 * (master_cpu_usage.used - master_cpu_usage_old.used)/(master_cpu_usage.total - master_cpu_usage_old.total));
		
		for(int i=0; i<CPU_CORES; i++){
			printf("cpu%d usage: %.2f%%\n", i,  100.0 * (cpu_core_usage[i].used - cpu_core_usage_old[i].used)/(cpu_core_usage[i].total - cpu_core_usage_old[i].total));
			cpu_core_usage_old[i] = cpu_core_usage[i];
		}
		master_cpu_usage_old = master_cpu_usage;
		sleep(3);

	};
        
	return 0;
}
