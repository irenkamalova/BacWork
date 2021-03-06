//
// Created by irisha on 05.10.15.
//

#ifndef BACWORK_QUEUEANDSOCKETS_H
#define BACWORK_QUEUEANDSOCKETS_H

#include "Module.h"
#include <vector>

class QueueAndSockets {
public:
    QueueAndSockets ();
    virtual ~QueueAndSockets ();
    void run(vector<Module> modules);
    static void receive_message(int number_of_current_pair);
    void module(Module *vals);
    int create_socket(int port, string ip_address);
    int create_sock_for_receiving(int port, string ip_address);
    void write_to_file(vector<Module> vals, int num_object);
private:
    static const int LENGTH_OF_ARRAY = 50;
    static const long long int TIME_SS = 10000000000; // 10 seconds
    static const long long int TIME = 10000000000; // 15 seconds
    static const long long int SLEEP_TIME = 100000;
    long long int array_for_file[203][200000];
    vector<Module> modules;

};

#endif //BACWORK_QUEUEANDSOCKETS_H
