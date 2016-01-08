#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int flag = 50;

void* DoWork(void* args) {
    int *id;
    id = (int *) args;
    int a = *id;
    //printf("ID: %d, CPU: %d\n", id, sched_getcpu());
    /*int n = 1, m = 1, l = 1;
    for(int i = 0; i < 10000; i++) {
    for(int k = 1; k < 10000; k++) {
        n = n * k;
        m = m * (n / k);
        l = l * (k - 1) + l * k;
    }
    n = 1; m = 1; l = 1;
    }*/
    printf("ID: %d, CPU: %d\n", a, sched_getcpu());
    while(flag == 50) {
        
        if(a != 0)
            usleep(0);
    }
    return 0;
}

int main() {   
    int thread_id[8];
    int numberOfProcessors = 8;
    printf("Number of processors: %d\n", numberOfProcessors);

    pthread_t threads[numberOfProcessors];

    pthread_attr_t attr;
    cpu_set_t cpus;
    pthread_attr_init(&attr);

    for (int i = 0; i < 1; i++) {
       thread_id[i] = i; 
       CPU_ZERO(&cpus);
       CPU_SET(i, &cpus);
       pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
       pthread_create(&threads[i], &attr, DoWork, (void *)&thread_id[i]);
    }
    pthread_attr_init(&attr);
    for(int i = 1; i < numberOfProcessors; i++) {
       thread_id[i] = i;
       pthread_create(&threads[i], &attr, DoWork, (void *)&thread_id[i]); 
    }
    sleep(3);
    flag = 1;
    for (int i = 0; i < numberOfProcessors; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
