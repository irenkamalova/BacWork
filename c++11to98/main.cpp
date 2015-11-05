#include <iostream>
#include "Module.h"
#include "QueueAndSockets.h"
#include <vector>
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h> 	 //clock_gettime()
#include <unistd.h>  //for sleep
#include <stdlib.h>  //atoi

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

uint64_t timestamp() {
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (static_cast<uint64_t>(ts.tv_sec) * 10000000u
			+ static_cast<uint64_t>(ts.tv_nsec) / 100u) * 100u;
}

uint64_t starttime;

string str = "messages_result.txt";
string s = "modules.txt";
static const long long int TIME_SS = 10000000000; // 10 seconds
static const long long int TIME = 10000000000;
static const long long int SLEEP_TIME = 500000;
long long int array_for_file[10][80000];

vector<Module> parser();
int create_socket(int *port, string *ip_address);
int create_sock_for_receiving(int *port, string *ip_address);
void* create_sockets_for_receiving(void *arg);
void* module(void * arg);
vector<pair<int*, int*> > pairs(10);
int datas[20][50];
const int LENGTH_OF_ARRAY = 50;

void write_into_file(Module * vals, ofstream *fout);

void receive_message(int& number_of_current_pair) {

	if(pairs[number_of_current_pair].second != &datas[number_of_current_pair][LENGTH_OF_ARRAY]) {
		pairs[number_of_current_pair].second = pairs[number_of_current_pair].second + 1;
	}
	else
		pairs[number_of_current_pair].second = &datas[number_of_current_pair][0];
}
struct receiver {
	virtual bool wait_for_message(int& number_of_current_pair_in) = 0;
	virtual bool there_message(int& number_of_current_pair_in) = 0;
};

struct receiver_queue : receiver {
	bool wait_for_message(int& number_of_current_pair_in) {
		return pairs[number_of_current_pair_in].first == pairs[number_of_current_pair_in].second ? true : false;
	}
	bool there_message(int& number_of_current_pair_in) {
		if(pairs[number_of_current_pair_in].first != pairs[number_of_current_pair_in].second) {
			receive_message(number_of_current_pair_in);
			return true;
		} else return false;
	}
};

struct receiver_socket : receiver {
	int checkout;
	bool wait_for_message(int& socket) {
		int result = 0;
		int r = recv(socket, &result, sizeof(int), MSG_DONTWAIT);
		checkout = r;
		return (r < 0) ? true : false;
	}
	bool there_message(int& socket) {
		int result = 0;
		if(checkout > 0) {
			checkout = recv(socket, &result, sizeof(int), MSG_DONTWAIT);
			return true;
		}
		else
			return false;
	}
};

struct sender {
	virtual void send_message(int& number_of_channel) = 0;
};

struct sender_queue : sender {
	void send_message(int& number_of_current_pair) {
		if(pairs[number_of_current_pair].first != &datas[number_of_current_pair][LENGTH_OF_ARRAY]) {
			pairs[number_of_current_pair].first = pairs[number_of_current_pair].first + 1;
		}
		else
			pairs[number_of_current_pair].first = &datas[number_of_current_pair][0];
	}
};

struct sender_socket : sender {
	void send_message(int& number_of_socket) {
		int result = 0;
		if (send(number_of_socket, &result, sizeof(int), 0) < 0) {
            handle_error("Error on send");
		}
	}
};

void * ss_module(void * arg) {
	//starttime = timestamp();
	sender_queue *sq = new sender_queue;
	int count_messages_ss = 0;
	int ss_channel = 0;
	int propusk = 0;
	long long int t_i = (long long int) starttime;

	while((long long int)(timestamp() - starttime) < TIME_SS) {

		int numeric_of_pair_for_output = 1; // but there can be more modules needs this signal
		for(int i = 0; i < numeric_of_pair_for_output; i++) {

			sq->send_message(ss_channel);
			count_messages_ss++;
		}
		t_i = t_i + SLEEP_TIME;
/*
		while( (t_i < (long long int)timestamp())  ) {
			t_i = t_i + SLEEP_TIME;
			for(int i = 0; i < numeric_of_pair_for_output; i++) {
			    sq->send_message(0);
			    count_messages_ss++;
		    }
			propusk++;
		}
*/
		//long long int for_usleep = (t_i - (long long int)timestamp()) / 1000;
		//uint64_t now_plus = timestamp() + for_usleep;
		while(  (t_i  > (long long int)timestamp() ) ) {
		    //usleep(0);
		}
	}
	cout << "AFTER SS END WORK" << endl;

	cout << count_messages_ss << endl;
	cout << propusk << endl;
}


int main(int argc, char *argv[]) {
	
	if (argc == 2) {
		int my_machine = atoi(argv[1]);
		vector<Module> modules = parser();
		 //modules for this machine
		vector<Module> my_modules;
		for(int i = 0; i < modules.size(); i++) {
			if(modules[i].get_machine() == my_machine) {
				my_modules.push_back(modules[i]);
			}
		}

		for(int i = 0; i < my_modules.size(); i++) {
			vector<Module::message_input> m_i = modules[i].get_all_message_input();
			for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
				if(it1->connection_type) {
					cout << it1->channel_from << endl;
				}
			}
		}
		//here we need to create channels for sending and receiving
		vector<pthread_t> threads;
		pthread_t thread;
		for(int i = 0; i < my_modules.size(); i++) {
			if(my_modules[i].get_port() != 0) {
				threads.push_back(thread);
				if (pthread_create(&threads[i], (pthread_attr_t *) NULL, create_sockets_for_receiving, &my_modules[i])) {
					handle_error("Error on thread create");
				}
			}
		}
		sleep(2);
		cout << "before connect " << endl;
		for(int i = 0; i < my_modules.size(); i++) {
			vector<Module::message_output> m_o = my_modules[i].get_all_message_output();
			for (int k = 0; k < m_o.size(); k++) {
				if (m_o[k].connection_type) {
					cout << m_o[k].channel_to << endl;
				}
			}
		}

		for(int i = 0; i < my_modules.size(); i++) {
			vector<Module::message_output> m_o = my_modules[i].get_all_message_output();
			for (int k = 0; k < m_o.size(); k++) {
				if (m_o[k].connection_type) {
					m_o[k].channel_to = create_socket(&m_o[k].port_to, &m_o[k].ip_address_to);
					my_modules[i].message_output_array[k].channel_to = m_o[k].channel_to;
				}
			}
		}
		cout << "after connect " << endl;
		for(int i = 0; i < my_modules.size(); i++) {
			vector<Module::message_output> m_o = my_modules[i].get_all_message_output();
			for (int k = 0; k < m_o.size(); k++) {
				if (m_o[k].connection_type) {
					cout << m_o[k].channel_to << endl;
				}
			}
		}

		for (vector<pthread_t>::iterator it = threads.begin(); it != threads.end();
					  ++it) {
			pthread_join(*it, (void **) NULL);
		}
		cout << "after join" << endl;
		for(int i = 0; i < my_modules.size(); i++) {
			vector<Module::message_input> m_i = my_modules[i].get_all_message_input();
			for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
				if(it1->connection_type) {
					cout << it1->channel_from << endl;
				}
			}
		}
        
        //initialisation
 		pthread_t ss_thread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		cpu_set_t cpus;
		int cpu_id = 0;		
		//int newprio = 100;
        //sched_param param;
              
        
		vector<pthread_t> thids;
		starttime = timestamp();
		for(int i = 0; i < my_modules.size(); i++) {
			thids.push_back(thread);
			if (pthread_create(&thids[i], (pthread_attr_t *) NULL, module, &my_modules[i])) {
				handle_error("Error on thread create");
			}
		}
        


		CPU_ZERO(&cpus);
        //for (int j = 0; j < 2; j++)
            CPU_SET(0, &cpus);
            //param.sched_priority = newprio;

        //pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
        //pthread_attr_setschedparam (&attr, &param);
        
		if (pthread_create(&ss_thread, &attr, ss_module, (void *) NULL)) {
			handle_error("Error on ss_thread create");
		}
        //pthread_setaffinity_np(ss_thread, sizeof(cpu_set_t), &cpus);
        pthread_join(ss_thread, (void **) NULL);

		for (vector<pthread_t>::iterator it = thids.begin(); it != thids.end();
			 ++it) {
			pthread_join(*it, (void **) NULL);
		}
		cout << "after join" << endl;
		for(int i = 0; i < my_modules.size(); i++) {
			int k = 0;
			int count_send = 0;
			int count_rec = 0;
			while(array_for_file[my_modules[i].get_number()][k] != 0) {
				if(array_for_file[my_modules[i].get_number()][k] == 1) {
					k++;
					count_send++;
				}
				else if(array_for_file[my_modules[i].get_number()][k] == 2) {
					k++;
					count_rec++;
				}
			}

			if(count_rec != 0)
				cout << my_modules[i].get_name() << " получил сообщений " << count_rec
				<< " цепочек " << count_rec / my_modules[i].get_nti() << endl;
			if(count_send != 0)
				cout << my_modules[i].get_name() << " отправил сообщений " << count_send
				<< " цепочек " << count_send / my_modules[i].get_nto() << endl;
		}
		//fout.close();
		cout << "finished" << endl;
		//QueueAndSockets *queueAndSockets = new QueueAndSockets;
		//queueAndSockets->run(my_modules);
		//delete(queueAndSockets);
//*/
	} else {
		cerr << "Wrong number of arguments. Input the number of your machine.";
		return 1;
	}
	return 0;
}

void * module (void * arg) {
    //uint64_t delay = timestamp() - starttime;
	Module * vals = (Module *) arg;
	int number_of_current_pair_in;
	int number_of_current_pair_out;
	double counter = 0.5;
	double current = vals->get_data_amount();
	short messages = 1;
	int index = 0;
	string name = vals->get_name();
	vector<Module::message_input> m_i = vals->get_all_message_input();
	vector<Module::message_output> m_o = vals->get_all_message_output();
    uint64_t delay = timestamp() - starttime;
	receiver *recv_object;
	sender *send_object;
	//cout << (long long int)(timestamp() - starttime) << endl;
	while((long long int)(timestamp() - starttime - delay) < TIME) {


		for (vector<Module::message_input>::iterator it = m_i.begin(); it != m_i.end(); ++it) {

			if (!it->connection_type)
				recv_object = new receiver_queue;
			else
				recv_object = new receiver_socket;
			//check if there any message. If no, switch thread
			while (recv_object->wait_for_message(it->channel_from)) {
				if((long long int)(timestamp() - starttime) < TIME) {
					if(!vals->get_affectation()) //if there no affectation
						usleep(0);
				}
				else break;
			}
			while (recv_object->there_message(it->channel_from)) {

				//receiving
				array_for_file[vals->get_number()][index] = 2; //bad
				//cout << index << endl;
				index++;
				for (int l = 0; l < it->time_hand; l++) {
					long long int result = 1;
					for (int k = 1; k <= 250; k++) {
						result = result * k;
					}
				}
				//cout << vals->get_name() << " received from " << it->name_from << endl;
				//sending
				if (it->parameter) {
					if(vals->get_data_amount() != 1) {
						if (current < counter) {
							messages = 0;
						}
						else {
							messages = 1;
							counter += 1.0;
						}
						current += vals->get_data_amount();
					}
					for(int m = 0; m < messages; m++) {
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
							}
							send_object->send_message(m_o[k].channel_to);
							array_for_file[vals->get_number()][index] = 1;
							//cout << index << endl;
							index++;
							delete (send_object);
							//cout << vals->get_name() << " sent to " << m_o[k].name_to << endl;
						}
					}
				}
			}
			delete(recv_object);
		}
	}
	//close sockets for receiving
	for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
		if (it1->connection_type) {
			close(it1->channel_from);
		}
	}
	cout << vals->get_name() << " finished " << endl;
}

vector<Module> parser() {
	char * cstr = new char [s.length()+1];
	strcpy(cstr, s.c_str());
	ifstream fin(cstr);
	char buff[SIZE];
	int i = 0;//number of modules
	int number_of_mes_input;
	int number_of_mes_output;

	vector<Module> vals;
	while (fin >> buff) {
		if (strcmp(buff, "-") == 0) {
			i--;
		} else {
			Module *module = new Module;
			module->set_name(buff);
			vals.push_back(*module);
			vals[i].set_number(i);
			number_of_mes_input = 0; // number of input data
			vals[i].set_nti(number_of_mes_input);
			number_of_mes_output = 0; // number of output data
			vals[i].set_nto(number_of_mes_output);

			int machine;
			fin >> machine;
			vals[i].set_machine(machine);

			int port;
			fin >> port;
			vals[i].set_port(port);

			bool aff;
			fin >> aff;
			vals[i].set_affectation(aff);

			double amount;
			fin >> amount;
			vals[i].set_data_amount(amount);

			string address;
			fin >> address;
			vals[i].set_my_ip_address(address);

		}
		fin >> buff;
		if (strcmp(buff, "-") != 0) {

			Module::message_input m_i;

			m_i.name = buff;

			int time_hand;
			fin >> time_hand;
			m_i.time_hand = time_hand;

			fin >> buff;
			m_i.name_from = buff;

			bool parameter;
			fin >> parameter;
			m_i.parameter = parameter;

			bool connection_type;
			fin >> connection_type;
			m_i.connection_type = connection_type;

			if(!connection_type) { //it means that connection type = queue
				int channel_number;
				fin >> channel_number;
				m_i.channel_from = channel_number;

			}
			else { 	 			  //connection type = socket
				string ip_address;
				fin >> ip_address;
				m_i.ip_address_from = ip_address;
			}

			vals[i].set_message_input(m_i, vals[i].get_nti());
			vals[i].inc_nti();
		}
		fin >> buff;
		if (strcmp(buff, "-") != 0) {

			Module::message_output m_o;

			m_o.name = buff;

			int time_form;
			fin >> time_form;
			m_o.time_form = time_form;

			fin >> buff;
			m_o.name_to = buff;

			bool connection_type;
			fin >> connection_type;
			m_o.connection_type = connection_type;

			if(!connection_type) { //it means that connection type = queue
				int channel_number;
				fin >> channel_number;
				m_o.channel_to = channel_number;
			}
			else { // type = socket
				int port_number;
				fin >> port_number;
				m_o.port_to = port_number;

				string ip_address;
				fin >> ip_address;
				m_o.ip_address_to = ip_address;
			}

			vals[i].set_message_output(m_o, vals[i].get_nto());
			vals[i].inc_nto();
		}
		i++;
	}

	fin.close();
	return vals;
}

int create_socket(int *port, string *ip_address) {
	int sock;
	struct sockaddr_in addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		handle_error("In function create_socket - socket:");
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(*port);
	const char *cstr = (*ip_address).c_str();
	addr.sin_addr.s_addr = inet_addr(cstr);
	if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		cerr << *port << endl;
		handle_error("In function create_socket - connect:");

	}
	return sock;
}

int create_sock_for_receiving(int *port, string *ip_address) {
	int sock;
	struct sockaddr_in addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		handle_error("Socket create:");
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(*port);
	const char *cstr = (*ip_address).c_str();
	addr.sin_addr.s_addr = inet_addr(cstr);
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		cerr << *port << endl;
		handle_error("Bind error:");
	}
	listen(sock, 50);
	return sock;
}

void* create_sockets_for_receiving(void *arg) {
	Module * vals = (Module *) arg;
	vector<Module::message_input> m_i = vals->get_all_message_input();
	int socket_for_receiving;
	if(vals->get_port() != 0) {
		int port = vals->get_port();
		string address = vals->get_my_ip_address();
		socket_for_receiving = create_sock_for_receiving(&port, &address);
	}
	int k = 0;
	for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
		if(it1->connection_type) { // type = socket
			it1->channel_from = accept(socket_for_receiving, NULL, NULL);
			if (it1->channel_from < 0) {
				cerr << vals->get_port() << endl;
				handle_error("Accept error:");
			}
			vals->message_input_array[k].channel_from = it1->channel_from;
			cout << vals->get_name() << " accepted " << it1->name_from << endl;
		}
		k++;
	}
}

void write_into_file(Module * vals, ofstream * fout) {
	int k = 0;
	int count_send = 0;
	int count_rec = 0;
	while(array_for_file[vals->get_number()][k] != 0) {
		if(array_for_file[vals->get_number()][k] == 1) {
			k++;
			count_send++;
		}
		else if(array_for_file[vals->get_number()][k] == 2) {
			k++;
			count_rec++;
		}
	}

	if(count_rec != 0)
		*fout << vals->get_name() << " получил сообщений " << count_rec
		<< " цепочек " << count_rec / vals->get_nti() << endl;
	if(count_send != 0)
		*fout << vals->get_name() << " отправил сообщений " << count_send
		<< " цепочек " << count_send / vals->get_nto() << endl;
}
