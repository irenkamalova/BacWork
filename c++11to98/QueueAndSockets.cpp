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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

QueueAndSockets::QueueAndSockets() { }

QueueAndSockets::~QueueAndSockets() { }
void write_to_file(vector<Module> vals, int num_object);
vector<pair<int*, int*> > pairs(10);
int datas[20][50];
const int LENGTH_OF_ARRAY = 50;



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
    int checkout;
    bool wait_for_message(int socket) {
        int result = 0;
        int r = recv(socket, &result, sizeof(int), MSG_DONTWAIT);
        checkout = r;
        return (r < 0) ? true : false;
    }
    bool there_message(int socket) {
        int result = 0;
        if(checkout > 0) {
            checkout = recv(socket, &result, sizeof(int), MSG_DONTWAIT);
            return true;
        }
        else
            return false;
    }
    void check() {
        cout << "socket" << endl;
    }
};

struct sender {
    virtual void send_message(int number_of_channel) = 0;
};

struct sender_queue : sender {
    void send_message(int number_of_current_pair) {
        if(pairs[number_of_current_pair].first != &datas[number_of_current_pair][LENGTH_OF_ARRAY]) {
            pairs[number_of_current_pair].first = pairs[number_of_current_pair].first + 1;
        }
        else
            pairs[number_of_current_pair].first = &datas[number_of_current_pair][0];
    }
};

struct sender_socket : sender {
    void send_message(int number_of_socket) {
        int result = 0;
        if (send(number_of_socket, &result, sizeof(int), 0) < 0) {
            //int err = errno;
            perror("sending");
            //if(err != ECONNRESET)
                exit(EXIT_FAILURE);

        }
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
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    QueueAndSockets *runner = my_data->runner;
    Module *arg = my_data->arg;
    runner->module(arg);
}

int sockets_array[20][20];


void QueueAndSockets::run(vector<Module> m) {
    vector<pthread_t> thids(SIZE);
    this->modules = m;
    struct thread_data threadarray[20];
    //create pairs
    vector<Module::message_input> m_i;
    for(int i = 0; i < modules.size(); i++) {
        m_i = modules[i].get_all_message_input();
        for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
            if(!it1->connection_type) { //type = queue
                pairs.push_back(make_pair(&datas[i][0], &datas[i][0]));
            }
        }
    }

    vector<Module*> m_addrs;
    for(int i = 0; i < modules.size(); i++) {
        m_addrs.push_back(&modules[i]);
    }

    for(int i = 0; i < modules.size(); i++) {
        threadarray[i].runner = this;
        threadarray[i].arg = m_addrs[i];
        if (pthread_create(&thids[i], (pthread_attr_t *) NULL, module_prom, &threadarray[i])) {
            cerr << "Error on thread create!\n";
            exit(EXIT_FAILURE);
        }
    }
    sleep(2); //we need all threads begin start their work before try to connect
    //create sockets

    for(int i = 0; i < modules.size(); i++) {
        vector<Module::message_output>* m_o = (*m_addrs[i]).get_address_of_messages_output();
        for (int k = 0; k < (*m_o).size(); k ++) {
            if ((*m_o)[k].connection_type) {
                (*m_o)[k].channel_to = create_socket((*m_o)[k].port_to, (*m_o)[k].ip_address_to);
                sockets_array[(*m_addrs[i]).get_number()][k] = (*m_o)[k].channel_to;
            }
        }
    }
    starttime = timestamp();
    //SS module
    sender_queue *sq = new sender_queue;
    long long int sleep_time = SLEEP_TIME;
    int index = 0;
    int propusk = 0;
    //starttime = timestamp();
    long long int t_i = (long long int) starttime;
    if((long long int)(timestamp() - starttime) < 0) {
        cout << "starttime error " << endl;
        exit(EXIT_FAILURE);
    }
    while((long long int)(timestamp() - starttime) < TIME) {
        if((long long int)(timestamp() - starttime) < 0) {
            cout << "Error 187 " << endl;
            exit(EXIT_FAILURE);
        }

        int numeric_of_pair_for_output = 1; // but there can be more modules needs this signal
        for(int i = 0; i < numeric_of_pair_for_output; i++) {
            sq->send_message(0);
            array_for_file[11][index++] = 1;
            //array_for_file[vals->get_index_for_file()][index++] = (long long int)(timestamp() - starttime);
        }
        t_i = t_i + sleep_time * 1000;
        while( (t_i - (long long int)timestamp()) < 0 ) {
            t_i = t_i + sleep_time * 1000;
            propusk++;
        }
        //cout << (t_i - (long long int)timestamp()) / 1000 << endl;
        if(t_i - (long long int)timestamp() < 0) {
            cout << "Error 205" << endl;
            exit(EXIT_FAILURE);
        }
        int for_usleep = (t_i - (long long int)timestamp()) / 1000;
        usleep( for_usleep );
    }
    cout << "AFTER SS END WORK" << endl;




    for (vector<pthread_t>::iterator it = thids.begin(); it != thids.end();
         ++it) {
        pthread_join(*it, (void **) NULL);
    }
    //close sockets
    for(int i = 0; i < modules.size(); i++) {
        vector<Module::message_output>* m_o = (*m_addrs[i]).get_address_of_messages_output();
        for (int k = 0; k < (*m_o).size(); k ++) {
            if ((*m_o)[k].connection_type) {
                close((*m_o)[k].channel_to);
            }
        }
    }

    array_for_file[modules.size()][0] = 0;
    write_to_file(m, 5);

    cout << "program finished" << endl;

}


void QueueAndSockets::module(Module *vals) {
    //cout << vals->get_name() << endl;
    int number_of_current_pair_in;
    int number_of_current_pair_out;
    int index = 0;
    string name = vals->get_name();
    vector<Module::message_input> m_i = vals->get_all_message_input();
    vector<Module::message_output> m_o = vals->get_all_message_output();
    //sockets for receiving

    int socket_for_receiving;
    if(vals->get_port() != 0)
        socket_for_receiving = create_sock_for_receiving(vals->get_port(), vals->get_my_ip_address());
    for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
        if(it1->connection_type) { // type = socket
            it1->channel_from = accept(socket_for_receiving, NULL, NULL);
            if (it1->channel_from < 0) {
                perror("accept");
                cerr << vals->get_port() << endl;
                exit(EXIT_FAILURE);
            }
            cout << vals->get_name() << " accepted " << it1->name_from << endl;
        }
    }


    sleep(5);

    receiver *recv_object;
    sender *send_object;
    //cout << (long long int)(timestamp() - starttime) << endl;
    while((long long int)(timestamp() - starttime) < TIME) {
        //cout << (long long int)(timestamp() - starttime) << endl;
        for (vector<Module::message_input>::iterator it = m_i.begin(); it != m_i.end(); ++it) {

            if (!it->connection_type)
                recv_object = new receiver_queue;
            else
                recv_object = new receiver_socket;
            //check if there any message. If no, switch thread
            while (recv_object->wait_for_message(it->channel_from)) {
                if((long long int)(timestamp() - starttime) < TIME)
                    usleep(0);
                else break;
            }
            while (recv_object->there_message(it->channel_from)) {
                //receiving
                array_for_file[vals->get_number()][index] = 2; //bad
                cout << array_for_file[vals->get_number()][index] << endl;
                index++;
                for (int l = 0; l < it->time_hand; l++) {
                    long long int result = 1;
                    for (int k = 1; k <= 250; k++) {
                        result = result * k;
                    }
                }
                cout << vals->get_name() << " received from " << it->name_from << endl;
                //sending
                if (it->parameter) {
                    for (int k = 0; k < m_o.size(); k++) {
                        for (int l = 0; l < m_o[k].time_form; l++) {
                            long long int result = 1;
                            for (int n = 1; n <= 250; n++) {
                                result = result * n;
                            }
                        }
                        if (!m_o[k].connection_type) { // type = queue
                            send_object = new sender_queue;
                        }
                        else { // type = socket
                            send_object = new sender_socket;
                            m_o[k].channel_to = sockets_array[vals->get_number()][k];
                        }
                        send_object->send_message(m_o[k].channel_to);
                        array_for_file[vals->get_index_for_file()][index++] = 1;
                        cout << vals->get_name() << " sent to " << m_o[k].name_to << endl;
                    }
                }
            }

        }
    }
    //close sockets for receiving
    for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
        if (it1->connection_type) {
            close(it1->channel_from);
        }
    }
    cout << vals->get_name() << " finished " << endl;

    //*/
}

void QueueAndSockets::receive_message(int number_of_current_pair) {

    if(pairs[number_of_current_pair].second != &datas[number_of_current_pair][LENGTH_OF_ARRAY]) {
        pairs[number_of_current_pair].second = pairs[number_of_current_pair].second + 1;
    }
    else
        pairs[number_of_current_pair].second = &datas[number_of_current_pair][0];
}

int QueueAndSockets::create_socket(int port, string ip_address) {
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
//    inet_aton("0.0.0.0", &(addr.sin_addr));
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("in function create_socket - connect");
        cerr << port << endl;
        exit(EXIT_FAILURE);
    }
    return sock;
}

int QueueAndSockets::create_sock_for_receiving(int port, string ip_address) {
    int sock;
    struct sockaddr_in addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    const char *cstr = ip_address.c_str();
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
//    inet_aton("0.0.0.0", &(addr.sin_addr));
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        cerr << port << endl;
        perror("bind");
        exit(EXIT_FAILURE);
    }
    listen(sock, 50);
    return sock;
}

void QueueAndSockets::write_to_file(vector<Module> vals, int num_object) {




    cout << "in " << num_object << endl;
    char buffer[4];
    sprintf(buffer, "%d", num_object);
    string num_obj(buffer);



    //string s = "./queue/result_queue" + num_obj + ".txt";
    //ofstream fout(s);
    string s2 = "/home/newuser/messages_queue" + num_obj + ".txt";
    char * cstr = new char [s2.length()+1];
    strcpy (cstr, s2.c_str());
    ofstream fout2(cstr);

    long long int time = 0;
    int nano_seconds = 0;
    int micro_seconds = 0;
    int mini_seconds = 0;
    int seconds = 0;
    int count_send = 0;
    int count_rec = 0;
    for(int i = 0; i < vals.size(); i++) {
        int k = 0;
        count_send = 0;
        count_rec = 0;
        vals[i].set_index_for_file(vals[i].get_number());
        while(array_for_file[vals[i].get_index_for_file()][k] != 0) {
            //fout << vals[i].get_name();
            if(array_for_file[vals[i].get_index_for_file()][k] == 1) {
                //fout << " отправил в ";
                k++;
                count_send++;
            }
            else if(array_for_file[vals[i].get_index_for_file()][k] == 2) {
                //fout << " получил в ";
                k++;
                count_rec++;
            }
            time = array_for_file[vals[i].get_index_for_file()][k];
            seconds = time / 1000000000;
            if(seconds > 0) {
                mini_seconds = time / 1000000 - seconds * 1000;
                micro_seconds = time / 1000 - mini_seconds * 1000 - seconds * 1000000;
                nano_seconds = time - micro_seconds * 1000 - mini_seconds * 1000000 - seconds * 1000000000;
            } else {
                mini_seconds = time / 1000000;
                micro_seconds = time / 1000 - mini_seconds * 1000;
                nano_seconds = time - micro_seconds * 1000 - mini_seconds * 1000000;
            }
            /*
            fout << seconds << " секунд " <<
                    mini_seconds << " милисекунд " <<
                    micro_seconds << " микросекунд " <<
                    nano_seconds << " наносекунд "
                    << endl;
            k++;
            */
        }
        //fout << endl;

        if(num_object != 0) {

            if(count_rec != 0)
                fout2 << vals[i].get_name() << " получил сообщений " << count_rec
                << " цепочек " << count_rec / vals[i].get_nti() << endl;
            if(count_send != 0)
                fout2 << vals[i].get_name() << " отправил сообщений " << count_send
                << " цепочек " << count_send / vals[i].get_nto() << endl;
        }
        else {
            if(vals[i].get_name() != "Рег")
                fout2 << vals[i].get_name() << " получил сообщений " << count_rec
                << " цепочек " << count_rec << endl;
            else
                fout2 << vals[i].get_name() << " отправил сообщений " << count_rec
                << " цепочек " << count_rec / 2 << endl;
        }
    }
}
