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

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

vector<Module> parser(string s);
void check_parser(vector<Module> vals, string s);
int create_socket(int port, string ip_address);
int create_sock_for_receiving(int port, string ip_address);
void* create_sockets_for_receiving(void *arg);

int main(int argc, char *argv[]) {
	const char* both_way = "queue_and_socket";
	
	if (argc == 2) {
		int my_machine = atoi(argv[1]);
		vector<Module> modules = parser("/home/irisha/modules.txt");
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
		int numbers;
		vector<pthread_t> threads;
		pthread_t thread;
		for(int i = 0; i < my_modules.size(); i++) {
			if(my_modules[i].get_port() != 0) {
				threads.push_back(thread);

				if (pthread_create(&threads[i], (pthread_attr_t *) NULL, create_sockets_for_receiving, &my_modules[i])) {
					cerr << "Error on thread create!\n";
					exit(EXIT_FAILURE);
				}
			}
		}
		sleep(2);

		for(int i = 0; i < my_modules.size(); i++) {
			vector<Module::message_output> m_o = my_modules[i].get_all_message_output();
			for (int k = 0; k < m_o.size(); k++) {
				if (m_o[k].connection_type) {
					m_o[k].channel_to = create_socket(m_o[k].port_to, m_o[k].ip_address_to);
					//sockets_array[(*m_addrs[i]).get_number()][k] = (*m_o)[k].channel_to;
				}
			}
		}
		//int sockets[my_modules.size()];
		cout << "here" << endl;
		vector<int> sockets(20);
		vector<int> * s = &sockets;
		void * result = 0;
		int thread_number = 0;
		for(int i = 0; i < my_modules.size(); i++) {
			if(my_modules[i].get_port() != 0) {
				pthread_join(threads[thread_number], &result);
				thread_number++;
				vector<int> * s = (vector<int> *) result;

				int l = 0;
				for(int k = 0; k < my_modules[i].get_nti(); k++) {
					if(my_modules[i].message_input_array[k].connection_type) {
						my_modules[i].message_input_array[k].connection_type = sockets[l];
						l++;
					}
				}
			}
		}
		cout << "after join" << endl;
		for(int i = 0; i < my_modules.size(); i++) {
			vector<Module::message_input> m_i = modules[i].get_all_message_input();
			for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
				if(it1->connection_type) {
					cout << it1->channel_from << endl;
				}
			}
		}
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

vector<Module> parser(string s) {
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

int create_socket(int port, string ip_address) {
	int sock;
	struct sockaddr_in addr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("in function create_socket - socket");
		exit(EXIT_FAILURE);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	const char *cstr = ip_address.c_str();
	addr.sin_addr.s_addr = inet_addr(cstr);
	if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("in function create_socket - connect");
		cerr << port << endl;
		exit(EXIT_FAILURE);
	}
	return sock;
}

int create_sock_for_receiving(int port, string ip_address) {
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
	addr.sin_addr.s_addr = inet_addr(cstr);
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		cerr << port << endl;
		perror("bind");
		exit(EXIT_FAILURE);
	}
	listen(sock, 50);
	return sock;
}

int socket_for_receiving = 0;
void* create_sockets_for_receiving(void *arg) {
	Module * vals = (Module *) arg;
	int *s;
	//int socket_for_receiving;
	vector<int> sockets;
	if(vals->get_port() != 0)
		socket_for_receiving = create_sock_for_receiving(vals->get_port(), vals->get_my_ip_address());
	for(int i = 0; vals->get_nti(); i++ ) {
		if(vals->message_input_array[i].connection_type) { // type = socket
			vals->message_input_array[i].channel_from = accept(socket_for_receiving, NULL, NULL);
			cout << vals->message_input_array[i].channel_from << endl;
			sockets.push_back(vals->message_input_array[i].channel_from);
			if (vals->message_input_array[i].channel_from < 0) {
				perror("accept");
				cerr << vals->get_port() << endl;
				exit(EXIT_FAILURE);
			}
			cout << vals->get_name() << " accepted " << vals->message_input_array[i].name_from << endl;
		}
	}
	cout << vals->get_name() << "finished" << endl;
	vector<int> * result = &sockets;
	s = &socket_for_receiving;
	return (void *) s;
}