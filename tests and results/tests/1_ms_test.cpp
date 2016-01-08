#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h> 	 //clock_gettime()
#include <unistd.h>  //for sleep
#include <stdlib.h>  //atoi
#include <sched.h>
#include <map>
#define BILLION 1000000000L
using namespace std;

uint64_t timestamp() {
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return static_cast<uint64_t>(ts.tv_sec) * BILLION + static_cast<uint64_t>(ts.tv_nsec);
}

long long int unsigned result;
void wait_n_microsec(int n) {
    int l, k;    
    for (l = 0; l < n; l++) {
		result = 1;
		for (k = 1; k <= 200; k++) {
			result = result * k;
		}
	}
}

void * ss_module(void * arg) {
	int newprio = 99;
    sched_param param; 
    param.sched_priority = newprio;   
    int pid = getpid();
    if(sched_setscheduler(pid, SCHED_FIFO, &param)) {
        perror("on setscheduler: ");
        exit(0);
    } 
    cout <<  "sched: " << sched_getscheduler(pid) << endl;
    uint64_t t1 = timestamp();
    wait_n_microsec(10000);
    uint64_t t2 = timestamp();
    cout << (t2 - t1)/1000 << endl; 
}

int main() {

	pthread_t ss_thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	cpu_set_t cpus;
	int cpu_id = 0;
	
	CPU_ZERO(&cpus);
	CPU_SET(cpu_id, &cpus);

	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);

	if (pthread_create(&ss_thread, &attr, ss_module, (void *) NULL)) {
		perror("Error on ss_thread create");
	}
    pthread_join(ss_thread, (void **) NULL);
   
    return 0;
}
