// This will assume you only have Three adaptors In final version
// this is not be the case and it will be changed to work with any number of adapter but as
// This is a PoC I am just testing that the technology will work and how it might look.

#include <sys/sysinfo.h>
#include <sys/utsname.h> 
#include <sys/vfs.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define INTERFACE_COUNT 3
struct network_usage {
    long receive;
    long transmit;
};

void getNetworkUsage(struct network_usage usage[INTERFACE_COUNT], char interface_names[INTERFACE_COUNT][128]) {
    
    FILE* fp = fopen("/proc/net/dev", "r");
    for(int i = 0; i < 2; i++) {
      while((char)fgetc(fp) != '\n');
    }
    for(int i =0; EOF != fscanf(fp, " %s %ld %*d %*d %*d %*d %*d %*d %*d %ld %*d %*d %*d %*d %*d %*d %*d\n",interface_names[i], &(usage[i].receive), &(usage[i].transmit)); i++) {
        interface_names[i][strlen(interface_names[i])-1] = '\0';
    }
}

int main() {
	
	struct network_usage usage[INTERFACE_COUNT];
	struct network_usage old_usage[INTERFACE_COUNT];
    char interface_names[INTERFACE_COUNT][128];
	
	getNetworkUsage(old_usage, interface_names);

    //this way might cause problems if the amount of interfaces change
	sleep(1);
	while(1) {
		printf("\nNetwork Usage\n");
		printf("-------------\n");

		getNetworkUsage(usage, interface_names);

        for(int i = 0; i < INTERFACE_COUNT; i++){
		    printf("%s Download: %ld kB/s\n", interface_names[i], (usage[i].receive - old_usage[i].receive)/1024);
		    printf("%s   Upload: %ld kB/s\n", interface_names[i], (usage[i].transmit - old_usage[i].transmit)/1024);
            old_usage[i] = usage[i];
        }

		
	
		sleep(1);

	};
        
	return 0;
}