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
int datas[20][50];
struct receiver {
    virtual bool wait_for_message(int number_of_current_pair_in) = 0;
    virtual bool there_message(int number_of_current_pair_in) = 0;
    virtual void check() = 0;
};

struct receiver_queue : receiver {
    bool wait_for_message(int number_of_current_pair_in) {
        return pairs[number_of_current_pair_in].first == pairs[number_of_current_pair_in].second ? true : false;
    }
    bool there_message(int number_of_current_pair_in) {
        if(pairs[number_of_current_pair_in].first != pairs[number_of_current_pair_in].second) {
            QueueAndSockets::receive_message(number_of_current_pair_in);
            return true;
        } else return false;
    }
    void check() {
        cout << "queue" << endl;
    }

};


struct receiver_socket : receiver {
    bool wait_for_message(int socket) {
        int result = 0;
        return (recv(socket, &result, sizeof(int), MSG_DONTWAIT) <  0) ? true : false;
    }
    bool there_message(int socket) {
        int result = 0;
        return (recv(socket, &result, sizeof(int), MSG_DONTWAIT) == 0) ? true : false;
    }
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

struct thread_data {
    QueueAndSockets *runner;
    Module *arg;
};

void* module_prom(void *threadarg) {
    cout << "in mod_prom" << endl;
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    QueueAndSockets *runner = my_data->runner;
    Module *arg = my_data->arg;
    runner->module(arg);
}


void QueueAndSockets::run(vector<Module> modules) {
    vector<pthread_t> thids(SIZE);
    struct thread_data threadarray[20];
    for(int i = 0; i < modules.size(); i++) {
        threadarray[i].runner = this;
        threadarray[i].arg = &modules[i];
        if (pthread_create(&thids[i], (pthread_attr_t *) NULL, module_prom, &threadarray[i])) {
            cerr << "Error on thread create!\n";
            exit(EXIT_FAILURE);
        }
    }
    sleep(5);
    for(vector<Module>::iterator it = modules.begin(); it != modules.end();
        ++it) {
        vector<Module::message_output> m_o = it->get_all_message_output();
        for (vector<Module::message_output>::iterator it2 = m_o.begin(); it2 != m_o.end(); ++it2) {
            if (it2->connection_type) {
                sleep(5);
                it2->socket_to = create_socket(it2->channel_to);

            }
        }
    }

    for (vector<pthread_t>::iterator it = thids.begin(); it != thids.end();
         ++it) {
        pthread_join(*it, (void **) NULL);
    }

    for(vector<Module>::iterator it = modules.begin(); it != modules.end();
        ++it) {
        vector<Module::message_output> m_o = it->get_all_message_output();
        for (vector<Module::message_output>::iterator it2 = m_o.begin(); it2 != m_o.end(); ++it2) {
            if (it2->connection_type) {
                close(it2->socket_to);

            }
        }
    }


}
long long int array_for_file[203][200000];

void QueueAndSockets::module(Module *vals) {
    cout << vals->get_name() << endl;
    int number_of_current_pair_in;
    int number_of_current_pair_out;
    int index = 0;
    vector<Module::message_input> m_i = vals->get_all_message_input();
    //sockets for receiving
    int socket_for_receiving;
    if(vals->get_port() != 0)
        socket_for_receiving = create_sock_for_receiving(vals->get_port());
    for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
        if(it1->connection_type) { // type = socket
            it1->socket_from = accept(socket_for_receiving, NULL, NULL);
            if (it1->socket_from < 0) {
                perror("accept");
                cerr << vals->get_port() << endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    cout << "I accepted all sockets!" << endl;

    //close sockets for receiving
    for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
        if (it1->connection_type) {
            close(it1->socket_from);
        }
    }

    /*
    receiver *recv_object;

    vector<Module::message_output> m_o = vals->get_all_message_output();
    for(vector<Module::message_input>::iterator it = m_i.begin(); it != m_i.end(); ++it ) {
        if(!it->connection_type)
            recv_object = new receiver_queue;
        else
            recv_object = new receiver_socket;

        while (recv_object->wait_for_message(number_of_current_pair_in)) {
            if(vals->get_time_for_sleep() != 1)
                usleep(0);
            if( (long long int)(timestamp() - starttime) < 10000000000 )
                break;
        }
        while(recv_object->there_message(number_of_current_pair_in)) {
            //receiving
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
     */
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

int QueueAndSockets::create_socket(int port) {
    int sock;
    struct sockaddr_in addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("in function create_socket - socket");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("in function create_socket - connect");
        cerr << port << endl;
        exit(EXIT_FAILURE);
    }
    return sock;
}

int QueueAndSockets::create_sock_for_receiving(int port) {
    int sock;
    struct sockaddr_in addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        cerr << port << endl;
        perror("bind");
        exit(EXIT_FAILURE);
    }
    listen(sock, 50);
    return sock;
}