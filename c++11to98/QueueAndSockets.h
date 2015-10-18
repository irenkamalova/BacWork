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
private:
    static const int LENGTH_OF_ARRAY = 50;
    static const long long int TIME = 15000000000;
    long long int array_for_file[203][200000];
    vector<Module> modules;

};

#endif //BACWORK_QUEUEANDSOCKETS_H
