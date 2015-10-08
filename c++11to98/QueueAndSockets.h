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
    void send_message(int number_of_current_pair);
    static void receive_message(int number_of_current_pair);
    void module(Module *vals);
    int create_socket(int port);
    int create_sock_for_receiving(int port);
    void create_sockets(vector<Module>* vals);
private:
    static const int LENGTH_OF_ARRAY = 50;

    long long int array_for_file[203][200000];
    vector<Module> modules;

};

#endif //BACWORK_QUEUEANDSOCKETS_H
