//
// Created by Irina on 05.10.15.
//
#include "Module.h"
#include "QueueAndSockets.h"
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <time.h> 	 //clock_gettime()
#include <unistd.h> //sleep
#include <stdlib.h> //exit
#include <cstring>
///// Library for sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

using namespace std;

QueueAndSockets::QueueAndSockets() { }

QueueAndSockets::~QueueAndSockets() { }
vector<pair<int*, int*> > pairs(10);

struct receiver {
    virtual bool wait_for_message(int number_of_current_pair_in) = 0;
    virtual void check() = 0;
};

struct receiver_queue : receiver {
    bool wait_for_message(int number_of_current_pair_in) {
        return pairs[number_of_current_pair_in].first == pairs[number_of_current_pair_in].second ? true : false;
    }
    void check() {
        cout << "queue" << endl;
    }

};

struct receiver_socket : receiver {
    bool wait_for_message(int number_of_current_pair_in) { return true; }
    void check() {
        cout << "socket" << endl;
    }
};

uint64_t timestamp() {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (static_cast<uint64_t>(ts.tv_sec) * 10000000u
            + static_cast<uint64_t>(ts.tv_nsec) / 100u) * 100u;
}

uint64_t starttime;

void QueueAndSockets::run(vector<Module> m) {
    for(vector<Module>::iterator it = m.begin(); it != m.end();
        ++it) {
        cout << it->get_name() << endl;
    }
    bool flag = false;
    receiver *object;
    if(flag) {
        object = new receiver_queue;
    }
    else object = new receiver_socket;

    object->check();

}

void QueueAndSockets::module(Module *vals) {
    int number_of_current_pair_in;
    int number_of_current_pair_out;
    int index = 0;
    receiver *recv_object;
    vector<Module::message_input> m_i = vals->get_all_message_input();
    vector<Module::message_output> m_o = vals->get_all_message_output();
    for(vector<Module::message_input>::iterator it = m_i.begin(); it != m_i.end(); ++it ) {
        if(!it->connection_type) { // type = queue
            recv_object = new receiver_queue;
        }
        while(recv)
            while (recv_object->wait_for_message(number_of_current_pair_in)) {
                if(vals->get_time_for_sleep() != 1)
                    usleep(0);
                if( (long long int)(timestamp() - starttime) < 10000000000 )
                    break;
            }
            while(pairs[number_of_current_pair_in].first != pairs[number_of_current_pair_in].second) {
                //receiving
                receive_message(number_of_current_pair_in);
                array_for_file[vals->get_number()][index++] = 2; //bad
                for(int l = 0; l < it->time_hand; l++) {
                    long long int result = 1;
                    for (int k = 1; k <= 250; k++) {
                        result = result * k;
                    }
                }

                //sending
                if(it->parameter) {
                    for(vector<Module::message_output>::iterator it1 = m_o.begin(); it1 != m_o.end(); ++it1 ) {
                        for(int l = 0; l < it1->time_form; l++) {
                            long long int result = 1;
                            for (int n = 1; n <= 250; n++) {
                                result = result * n;
                            }
                        }
                        if(!it1->connection_type) { // type = queue
                            number_of_current_pair_out = it1->channel_to;
                            send_message(number_of_current_pair_out);
                            array_for_file[vals->get_index_for_file()][index++] = 1;
                        }
                        else { // type = socket

                        }
                    }
                }
            }

    }
}

void QueueAndSockets::send_message(int number_of_current_pair) {
    if(pairs[number_of_current_pair].first != &datas[number_of_current_pair][LENGTH_OF_ARRAY]) {
        pairs[number_of_current_pair].first = pairs[number_of_current_pair].first + 1;
    }
    else
        pairs[number_of_current_pair].first = &datas[number_of_current_pair][0];
}

void QueueAndSockets::receive_message(int number_of_current_pair) {

    if(pairs[number_of_current_pair].second != &datas[number_of_current_pair][LENGTH_OF_ARRAY]) {
        pairs[number_of_current_pair].second = pairs[number_of_current_pair].second + 1;
    }
    else
        pairs[number_of_current_pair].second = &datas[number_of_current_pair][0];
}
