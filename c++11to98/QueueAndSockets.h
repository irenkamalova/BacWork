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
    void receive_message(int number_of_current_pair);
    void module(Module *vals);

private:
    static const int LENGTH_OF_ARRAY = 50;
    int datas[20][LENGTH_OF_ARRAY];
    long long int array_for_file[203][200000];


};

#endif //BACWORK_QUEUEANDSOCKETS_H
