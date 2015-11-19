
#include "Socket.h"
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <time.h> 	 //clock_gettime()
#include <unistd.h> //sleep
//////////////////// for sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <set>

Socket::Socket() {
    // TODO Auto-generated constructor stub

}

Socket::~Socket() {
    // TODO Auto-generated destructor stub
}

struct thread_data {
    Queue *runner;
    Module *arg;
};

struct thread_data_s {
    Queue *runner;
    vector<Module> vals;
};


void *module(void *threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    Queue *runner = my_data->runner;
    Module *arg = my_data->arg;
    runner->module_queue(arg);
}
void *modules(void *threadarg) {
    struct thread_data_s *my_data;
    my_data = (struct thread_data_s *) threadarg;
    Queue *runner = my_data->runner;
    vector<Module> vals = my_data->vals;
    runner->modules_queue(vals);
}

void module(Socket *runner, Module *arg) {
    runner->module_socket(arg);
}

void modules(Socket *runner, vector<Module>* vals) {
    runner->modules_sockets(vals);
}


uint64_t Socket::timestamp() {
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (static_cast<uint64_t>(ts.tv_sec) * 10000000u
            + static_cast<uint64_t>(ts.tv_nsec) / 100u) * 100u;
}

void Socket::set_general(vector<Module> arg) {
    int i = 0;
    port = PORT;
    for( ; i < arg.size(); i++) {
        arg[i].set_index_for_file(i);
        arg[i].set_port(port);
        arg[i].set_n_of_s(0);
        arg[i].set_n_of_s_i(0);
        port++;
    }
    set_index_for_file(i);
    this->general = arg;
}

void Socket::create_ports(vector<Module> vals, int num_object, modules_types m_t) {
    int size = vals.size();
    for(int i = 0; i < size; i++) {
        vals[i].set_port(port);
        port++;
        vals[i].set_n_of_s(0);
        vals[i].set_n_of_s_i(0);
        vals[i].set_index_for_file(index_for_file);
        index_for_file++;
    }
    if(m_t == AUTO) {
        auto_accomp.push_back(vals);
    }
    else if(m_t == SEARCH) {
        search.push_back(vals);
    }
    else exit(1);
}

void Socket::create_sockets(vector<Module>* vals, int num_object, string s) {

    int size = vals->size();
    int gen_size = general.size();
    for(int i = 0; i < size; i++ ) {
        //cout << (*vals)[i].get_name() << endl;
        for(int j = i + 1; j < size; j++) {
            for(int k = 0; k < (*vals)[j].get_nti(); k++) {
                if((*vals)[i].get_dto(0) == (*vals)[j].get_dti(k)) {
                    int current_socket = create_socket( (*vals)[j].get_port() );
                    //cout << (*vals)[i].get_n_of_s() << current_socket << endl;
                    (*vals)[i].set_socket_for_send((*vals)[i].get_n_of_s(), current_socket);
                    (*vals)[i].inc_n_of_s();
                    (*vals)[j].inc_n_of_s_i();
                }
            }
        }
        for(int j = 0; j < gen_size; j++) {
            for(int k = 0; k < general[j].get_nti(); k++) {
                //cout << (*vals)[i].get_dto(0) << " " << general[j].get_dti(k) << endl;
                if((*vals)[i].get_dto(0) == general[j].get_dti(k)) {
                    //cout << (*vals)[i].get_name() << endl << (*vals)[i].get_dto(0) << " " << general[j].get_dti(k) << endl;
                    int current_socket = create_socket(general[j].get_port() );
                    (*vals)[i].set_socket_for_send((*vals)[i].get_n_of_s(), current_socket );
                    (*vals)[i].inc_n_of_s();
                    general[j].inc_n_of_s_i();
                }
            }
        }
    }
    string file_name = "./socket/sockets_" + s + to_string(num_object) + ".txt";
    ofstream fout(file_name);
    for(int i = 0; i < size; i++) {
        fout << (*vals)[i].get_name() << " ";
        for(int j = 0; j < (*vals)[i].get_n_of_s(); j++) {
            fout << (*vals)[i].get_socket_for_send(j) << " ";
        }
        fout << "число сокетов для приема сообщения: " << (*vals)[i].get_n_of_s_i() << endl;
    }
    fout.close();
}

int Socket::run(int flows_auto, int flows_search) {
    //dump(general, "general", 0);
    cout << "Socket run\n";
    string s2 = "/home/irisha/.clion11/system/cmake/generated/8678f0dd/8678f0dd/Debug/socket/dump_from_socket.txt";
    char * cstr = new char [s2.length()+1];
    strcpy (cstr, s2.c_str());
    ofstream fout(cstr);
    for(Module vals: general) {
        fout << vals.get_name() << " " << vals.get_port() << endl;
    }
    for(vector<Module> values: auto_accomp) {
        for(auto& vals: values) {
            fout << vals.get_name() << " " << vals.get_port() << endl;
        }
    }
    for(vector<Module> values: search) {
        for(auto& vals: values) {
            fout << vals.get_name() << " " << vals.get_port() << endl;
        }
    }
    fout.close();
    for(int i = 0; i < 203; i++) {
        for(int k = 0; k < 200000; k++)
            array_for_file[i][k] = 0;
    }
    vector<pthread_t> generals_threads(3);
    vector<pthread_t> thids(100);
    starttime = timestamp();

    struct thread_data_s thrdarrays[200];
    struct thread_data thrdarray[20];

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    int cpu_id = 0;

    for(int i = 0; i < general.size(); i++ ) {
        thrdarray[i].runner = this;
        thrdarray[i].arg = &general[i];

        if(general[i].get_time_for_sleep() == 1) {
            CPU_ZERO(&cpus);
            CPU_SET(cpu_id, &cpus);
            pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
            if(pthread_create(&generals_threads[i], &attr, module,
                              (void *)&thrdarray[i])) {
                cerr << "Error on thread create!\n";
                exit(EXIT_FAILURE);
            }
            cpu_id++;
        }
        else
        if (pthread_create(&generals_threads[i], (pthread_attr_t *) NULL, module,
                           (void *)&thrdarray[i])) {
            cerr << "Error on thread create!\n";
            exit(EXIT_FAILURE);
        }
    }
    int k = 1;
    for(int i = 0; i < flows_auto; i++) {
        thrdarrays[k - 1].runner = this;
        thrdarrays[k - 1].vals = auto_accomp[i];
        if (pthread_create(&thids[k - 1], (pthread_attr_t *) NULL, modules,
                           (void *)&thrdarrays[k - 1])) {
            cerr << "Error on thread create!\n";
            exit(EXIT_FAILURE);
        }
        k++;
    }

    for(int i = 0; i < flows_search; i++) {
        thrdarrays[k - 1].runner = this;
        thrdarrays[k - 1].vals = search[i];
        if (pthread_create(&thids[k - 1], (pthread_attr_t *) NULL, modules,
                           (void *)&thrdarrays[k - 1])) {
            cerr << "Error on thread create!\n";
            exit(EXIT_FAILURE);
        }
        k++;
    }
    sleep(1);
    int k = 1;
    for(int i = 0; i < flows_auto; i++) {
        create_sockets(&auto_accomp[i], k, "auto");
        k++;
    }
    for(int i = 0; i < flows_search; i++) {
        create_sockets(&search[i], k, "search");
        k++;
    }
    //cout << "join_general\n";
    for (auto& thr : generals_threads) {
        thr.join();
    }
    //cout << "join_thids\n";
    for (auto& thr : thids) {
        thr.join();
    }
    cout << "after_join_all\n";
    k = 0;
    write_to_file(&general, k);
    //cout << "general_write\n";
    k++;
    for(int i = 0; i < flows_auto; i++ ) {
        write_to_file(&auto_accomp[i], k);
        k++;
        //cout << "auto_write\n";
    }
    for(int i = 0; i < flows_search; i++ ) {
        write_to_file(&search[i], k);
        k++;
        //cout << "search_write\n";
    }
    return 0;
}

void Socket::modules_sockets(vector<Module>* vals) {
    int size = vals->size();
    vector<thread> thids;
    for(int i = 0; i < size; i++) {
        thids.push_back(thread(module, this, & ( (*vals)[i] ) ) );
    }
    //sleep(2);
    //cout << num_obj << " join\n";
    for (auto& thr : thids) {
        thr.join();
    }
    //cout << num_obj << " afterjoin\n";
}

void Socket::module_socket(Module *arguments) {
    long long int result = 1;
    bool flag1 = true;
    int count = 0;
    int sock2 = create_sock_for_receiving(arguments->get_port());
    sleep(3);
    long long int t_i = (long long int) starttime;
    bool ifsend1 = false;
    double counter = 0.5;
    double current = arguments->get_data_amount();
    int sock1[SIZE] = {0};

    if (arguments->get_dti(0) != "СС") {
        for(int i = 0; i < arguments->get_n_of_s_i(); i++) {
            sock1[i] = accept(sock2, NULL, NULL);
            if (sock1[i] < 0) {
                perror("accept");
                cerr << arguments->get_port() << endl;
                //close_all_sockets(arg);
                exit(EXIT_FAILURE);
            }
        }
    }
    set<int> kitten;

    //cout << arguments->get_dti(0) << endl;
    if ((arguments->get_name() != "ФВД_АС" ) && (arguments->get_name() != "ФВД_П" )) {
        while ( (long long int)( timestamp() - starttime) < 13000000000) {
            for(int i = 0; i < arguments->get_n_of_s_i(); i++) {
                if (recv(sock1[i], &result, sizeof(long long int), MSG_DONTWAIT) <  0) {
                }
                else {
                    array_for_file[arguments->get_index_for_file()][count++] = 2;
                    array_for_file[arguments->get_index_for_file()][count++] = (long long int)(timestamp() - starttime);
                    for (int l = 0; l < arguments->get_th(i); l++) {
                        result = 1;
                        for (int k = 1; k <= 250; k++) {
                            result = result * k;
                        }
                    }
                    kitten.insert(i);
                }
            }
            string out = arguments->get_dto(0);
            //если выходные данные есть
            //if(sends_count > arguments->get_n_of_s_i())
            //    sends_count = arguments->get_n_of_s_i();
            if( (out != "") && (kitten.size() == arguments->get_n_of_s_i()) ) {
                kitten.clear();
                //cout << << "kit_size " << kitten.size() << endl;
                for(int r = 0; r < arguments->get_n_of_s(); r++) {
                    //мы их формируем
                    for(int m = 0; m < arguments->get_tf(0); m++) {
                        result = 1;
                        for(int k = 1; k <= 250; k++){
                            result = result * k;
                        }
                    }
                    //и вот теперь отправляем
                    //cout << "in sending loop\n";
                    array_for_file[arguments->get_index_for_file()][count++] = 1;
                    array_for_file[arguments->get_index_for_file()][count++] = (long long int)(timestamp() - starttime);
                    //cout << arguments->get_socket_for_send(i) << endl;
                    if (send(arguments->get_socket_for_send(r), &result, sizeof(long long int), 0) < 0) {
                        int err = errno;
                        cerr << arguments->get_name() << endl;
                        perror("sending");
                        if(err != ECONNRESET)
                            exit(EXIT_FAILURE);

                    }
                    //cout << arguments->get_name() << "send\n";
                }
                usleep(arguments->get_time_for_sleep());
            }
        }
    }
    else { //ФВД_АС или ФВД_П
        while ( (long long int)( timestamp() - starttime) < 13000000000) {
            //cout << arguments->get_name() << endl;
            //счетчики для принятия решения об отправлении данных
            while(!ifsend1) {
                if(current < counter) {
                    ifsend1 = false;
                }
                else {
                    ifsend1 = true;
                    counter += 1.0;
                }
                current += arguments->get_data_amount();
                if(!ifsend1) {

                    usleep(arguments->get_time_for_sleep());
                }
            }
            //мы их формируем
            for(int m = 0; m < arguments->get_tf(0); m++) {
                result = 1;
                for(int k = 1; k <= 250; k++){
                    result = result * k;
                }
            }
            //cout << arguments->get_n_of_s() << endl;
            for(int i = 0; i < arguments->get_n_of_s(); i++) {


                //и вот теперь отправляем
                //cout << "in sending loop\n";
                array_for_file[arguments->get_index_for_file()][count++] = 1;
                array_for_file[arguments->get_index_for_file()][count++] = (long long int)(timestamp() - starttime);
                //cout << arguments->get_socket_for_send(i) << endl;
                if (send(arguments->get_socket_for_send(i), &result, sizeof(long long int), 0) < 0) {
                    int err = errno;
                    puts("send error");
                    cerr << arguments->get_name() << endl;
                    perror("sending");
                    if(err != ECONNRESET)
                        exit(EXIT_FAILURE);
                }
                //cout << arguments->get_name() << "send\n";
            }
            t_i = t_i + arguments->get_time_for_sleep() * 1000;
            while( (t_i - (long long int)timestamp()) < 0 ) {
                t_i = t_i + arguments->get_time_for_sleep() * 1000;
            }
            //cout << (t_i - (long long int)timestamp()) / 1000 << endl;
            usleep( (t_i - (long long int)timestamp()) / 1000 );
            ifsend1 = false;
        }
    }
    for(int i = 0; i < arguments->get_n_of_s_i(); i++) {
        close(sock1[i]);
    }
    for(int i = 0; i < arguments->get_n_of_s(); i++) {
        close(arguments->get_socket_for_send(i));
    }
    close(sock2);
//	cout << arguments->get_name() << endl;
}

int Socket::create_socket(int port) {
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

void Socket::write_to_file(vector<Module>* vals, int num_object) {
    char buffer[4];
    sprintf(buffer, "%d", num_object);
    string num_obj(buffer);

    string s = "./socket/result_sock" + num_obj + ".txt";
    string s2 = "./socket/messages_sock" + num_obj + ".txt";
    ofstream fout(s);
    ofstream fout2(s2);
    long long int time = 0;
    int nano_seconds = 0;
    int micro_seconds = 0;
    int mini_seconds = 0;
    int seconds = 0;
    int count_send = 0;
    int count_rec = 0;
    for(int i = 0; i < vals->size(); i++) {
        int k = 0;
        count_send = 0;
        count_rec = 0;
        while(array_for_file[(*vals)[i].get_index_for_file()][k] != 0) {
            fout << (*vals)[i].get_name();
            if(array_for_file[(*vals)[i].get_index_for_file()][k] == 1) {
                fout << " отправил в ";
                k++;
                count_send++;
            }
            else if(array_for_file[(*vals)[i].get_index_for_file()][k] == 2) {
                fout << " получил в ";
                k++;
                count_rec++;
            }

            time = array_for_file[(*vals)[i].get_index_for_file()][k];
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
            fout << seconds << " секунд " <<
            mini_seconds << " милисекунд " <<
            micro_seconds << " микросекунд " <<
            nano_seconds << " наносекунд "
            << endl;
            k++;

        }
        fout << endl;
        if(num_object != 0) {

            if(count_rec != 0)
            if((*vals)[i].get_n_of_s_i() != 0)
                fout2 << (*vals)[i].get_name() << " получил сообщений " << count_rec
                << " цепочек " << count_rec / (*vals)[i].get_n_of_s_i() << endl;
            else
                cout << " в " << (*vals)[i].get_name() << " ошибка! Он получил сообщения, хотя не должен был" << endl;
            if(count_send != 0)
            if((*vals)[i].get_n_of_s() != 0)
                fout2 << (*vals)[i].get_name() << " отправил сообщений " << count_send
                << " цепочек " << count_send / (*vals)[i].get_n_of_s() << endl;
            else
                cout << " в " << (*vals)[i].get_name() << " ошибка! Он отправил сообщения, хотя не должен был" << endl;
        }
        else {
            if((*vals)[i].get_name() != "Рег")
                fout2 << (*vals)[i].get_name() << " получил сообщений " << count_rec
                << " цепочек " << count_rec << endl;
            else
                fout2 << (*vals)[i].get_name() << " отправил сообщений " << count_rec
                << " цепочек " << count_rec / 2 << endl;
        }
    }
}

void Socket::set_index_for_file(int index) {
    index_for_file = index;
}

int Socket::create_sock_for_receiving(int port) {
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
