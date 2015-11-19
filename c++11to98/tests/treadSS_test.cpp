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
#include <ctime>


using namespace std;

uint64_t timestamp() {
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (static_cast<uint64_t>(ts.tv_sec) * 10000000u
			+ static_cast<uint64_t>(ts.tv_nsec) / 100u) * 100u;
}

void * ss_module(void * arg) {
    clock_t starttime = clock();
    cout << "starttime " << starttime << endl;
    cout << "start in sec " << starttime/CLOCKS_PER_SEC << endl;
	int count_messages_ss = 0;
	int ss_channel = 0;
	int ss_channel2 = 7;
	int propusk = 0;
	int index = 0;
	int k = 0;
	int array_if_indexes[10000];
	clock_t t_i = starttime;
	cout << "curr clock " << endl;
	cout << "clock() - starttime " << clock() - starttime << endl;
	cout << "in sec ";
    
	while((clock() - starttime)/CLOCKS_PER_SEC < 10.0) {
	
	    t_i = clock();
	    //cout << "t_i " << t_i << endl;
	    count_messages_ss++;
        //cout << "clock() - t_i " << clock() - t_i << endl;
        //cout << "in sec " << (clock() - t_i)/CLOCKS_PER_SEC << endl;
        if((double)((clock() - t_i)/(CLOCKS_PER_SEC/1000.0)) > 1.0)
            propusk++;
        //cout <<  (clock() - t_i)/(CLOCKS_PER_SEC/1000.0) << endl;
        //cout << (clock() - starttime)/CLOCKS_PER_SEC << endl;   
		while( (double)((clock() - t_i)/(CLOCKS_PER_SEC/1000.0)) < 1.0 ) {
		    //cout << "clock() - t_i " << clock() - t_i << endl;
            //cout << "in sec " << (clock() - t_i)/CLOCKS_PER_SEC << endl;
		    //usleep(0);
		}
	}
	cout << "AFTER SS END WORK" << endl;

	cout << count_messages_ss << endl;
	cout << propusk << endl;
//	for(int i = 0; array_if_indexes[i] != 0; i++) {
//	    cout << array_if_indexes[i] << " " ;
//	}
	cout << endl;
}

int main(int argc, char *argv[]) {

        
        clock_t t1 = clock();
        clock();
        clock_t t2 = clock();
        cout << "number of clocks beetween clocks" << t2 - t1 << endl;       
        uint64_t time;
        t1 = clock();
        time = timestamp();  
        time = timestamp();      
        t2 = clock();
        cout << "number of clocks beetween timestamp" << t2 - t1 << endl; 
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
