#include <iostream>
#include "Module.h"
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h> 	 //clock_gettime()
#include <unistd.h>  //for sleep
#include <stdlib.h>  //atoi
#include <sched.h>
#include <map>
#define BILLION 1000000000L

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

uint64_t timestamp() {
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return static_cast<uint64_t>(ts.tv_sec) * BILLION + static_cast<uint64_t>(ts.tv_nsec);
}

void wait_n_microsec(int n) {
    int l, k;
    long long int unsigned result;
    for (l = 0; l < n; l++) {
		result = 1;
		for (k = 1; k <= 225; k++) {
			result = result * k;
		}
	}
}

vector<Module> parser1();
map<string, int> parser2();
map<int, string> parser3();
const int PORT = 60000;
const int N_GRIDS = 20000; //! for information array about receiving and sending messages
uint64_t starttime;

string str = "messages_result.txt";
string s = "modules.txt";
string s1 = "/home/newuser/ClionProjects/BacWork/c++11to98/file1.txt";
string s2 = "/home/newuser/ClionProjects/BacWork/c++11to98/file2.txt";
string s3 = "/home/newuser/ClionProjects/BacWork/c++11to98/file3.txt";
static const uint64_t TIME_SS = 10000000000; // 10 seconds
static const uint64_t TIME = 10000000000;
static const uint64_t SLEEP_TIME = 1000000;
static const short THREAD_SLEEP = 10;
static const short NUMBER_OF_QUEUE_CH = 50;
static const short NUMBER_OF_MODULES = 30;
static const int LENGTH_OF_ARRAY = 1500;
int array_for_file[NUMBER_OF_MODULES][70000];
uint64_t array_of_max_queue[100];
uint64_t array_of_queue[NUMBER_OF_MODULES][N_GRIDS];
//vector<map<int, long long int> >

vector<Module> parser();
int create_socket(int *port, string *ip_address);
int create_sock_for_receiving(int *port, string *ip_address);
void* create_sockets_for_receiving(void *arg);
void* module(void * arg);
vector<pair<int*, int*> > pairs;
int datas[NUMBER_OF_QUEUE_CH][LENGTH_OF_ARRAY];
vector<int> ss_channels;


void receive_message(int& number_of_current_pair) {

	if(pairs[number_of_current_pair].second != &datas[number_of_current_pair][LENGTH_OF_ARRAY - 1]) {
		pairs[number_of_current_pair].second = pairs[number_of_current_pair].second + 1;
	}
	else {
		pairs[number_of_current_pair].second = &datas[number_of_current_pair][0];
    }
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
	virtual int send_message(int& number_of_channel) = 0;
};

struct sender_queue : sender {
	int send_message(int& number_of_current_pair) {
		//cout << "send in " << number_of_current_pair << " first is " <<
		//		pairs[number_of_current_pair].first << endl;
		if(pairs[number_of_current_pair].first != &datas[number_of_current_pair][LENGTH_OF_ARRAY - 1]) {
			pairs[number_of_current_pair].first = pairs[number_of_current_pair].first + 1;
		//	cout << "send in " << number_of_current_pair << " first now is " <<
		//	pairs[number_of_current_pair].first << endl;
		}
		else
			pairs[number_of_current_pair].first = &datas[number_of_current_pair][0];
		return 0;
	}
};

struct sender_socket : sender {
	int send_message(int& number_of_socket) {
		int result = 0;
		if (send(number_of_socket, &result, sizeof(int), 0) < 0) {
            perror("Error on send");
			return -1;
		}
		return 0;
	}
};

void * ss_module(void * arg) {

	sender_queue *sq = new sender_queue;
	int count_messages_ss = 0;
	int propusk = 0;
	int index = 0;
	int k = 0;
	int array_if_indexes[5000];
	uint64_t t_i = starttime;
	cout << ss_channels.size() << endl;
	for(int i = 0; i < ss_channels.size(); i++) {
		cout << ss_channels[i] << endl;
	}

	while((timestamp() - starttime) < TIME_SS) {
        index++;

		for(int i = 0; i < ss_channels.size(); i++) {
			sq->send_message(ss_channels[i]);
			count_messages_ss++;
		}

		t_i = t_i + SLEEP_TIME;

		if( (t_i < timestamp())  ) {
			propusk++;
			array_if_indexes[k] = index;
			k++;
		}

		while(  (t_i  > timestamp() ) ) {
		    //usleep(0);
		}
	}
	cout << "AFTER SS END WORK" << endl;
    delete(sq);
	cout << count_messages_ss << endl;
	cout << propusk << endl;
	for(int i = 0; array_if_indexes[i] != 0; i++) {
	    cout << array_if_indexes[i] << " " ;
	}
	cout << endl;
}


int main(int argc, char *argv[]) {
	
	if (argc == 3) {

		for(int k = 0; k < NUMBER_OF_MODULES; k++)
			for(int j = 0; j < N_GRIDS; j++)
				array_for_file[k][j] = 0;


		int my_machine = atoi(argv[2]);

		vector<Module> modules = parser1();
		map<string, int> module_machine = parser2();
		map<int, string> machine_address = parser3();
		int port = PORT;

		for(int i = 0; i < modules.size(); i++) {
			modules[i].set_machine(module_machine[modules[i].get_name()]);
			modules[i].set_my_ip_address(machine_address[modules[i].get_machine()]);
		}

		for(int i = 0; i < modules.size(); i++) {
			for(int i_m = 0; i_m < modules[i].get_nto(); i_m++) {
				modules[i].message_output_array[i_m];
				for(int j = i + 1; j < modules.size(); j++) {
					for(int j_m = 0; j_m < modules[j].get_nti(); j_m++) {

						if( (modules[i].message_output_array[i_m].name == modules[j].message_input_array[j_m].name)
							&& (modules[i].message_output_array[i_m].name_to == modules[j].get_name()) ) {
							if(modules[i].get_machine() == modules[j].get_machine()) {
								modules[i].message_output_array[i_m].connection_type = 0;
								modules[j].message_input_array[j_m].connection_type = 0;
								pairs.push_back(make_pair(&datas[pairs.size() - 1][0], &datas[pairs.size() - 1][0]));
								modules[i].message_output_array[i_m].channel_to = pairs.size() - 1;
								modules[j].message_input_array[j_m].channel_from = pairs.size() - 1;
							}
							else {
								modules[i].message_output_array[i_m].connection_type = 1;
								modules[j].message_input_array[j_m].connection_type = 1;
								modules[j].set_port(port++);
								modules[i].message_output_array[i_m].port_to = modules[j].get_port();
								modules[i].message_output_array[i_m].ip_address_to = modules[j].get_my_ip_address();
								modules[j].message_input_array[j_m].ip_address_from = modules[i].get_my_ip_address();
							}
						}
					}
				}
			}
		}
		 //modules for this machine
		vector<Module> my_modules;
		for(int i = 0; i < modules.size(); i++) {
			if(modules[i].get_machine() == my_machine) {
				my_modules.push_back(modules[i]);
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


		for(int i = 0; i < my_modules.size(); i++) {
			vector<Module::message_output> m_o = my_modules[i].get_all_message_output();
			for (int k = 0; k < m_o.size(); k++) {
				if (m_o[k].connection_type) {
					m_o[k].channel_to = create_socket(&m_o[k].port_to, &m_o[k].ip_address_to);
					my_modules[i].message_output_array[k].channel_to = m_o[k].channel_to;
				}
			}
		}

		for (vector<pthread_t>::iterator it = threads.begin(); it != threads.end();
					  ++it) {
			pthread_join(*it, (void **) NULL);
		}
		cout << "after join" << endl;

		//initialisation
		pthread_t ss_thread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		cpu_set_t cpus;
		int cpu_id = 0;
		CPU_ZERO(&cpus);
		CPU_SET(cpu_id, &cpus);
		int p = 1;
		//param.sched_priority = newprio;
		if(argv[1]) { //if on SS module on this machine
			for (int i = 0; i < my_modules.size(); i++) {
				for (int i_m = 0; i_m < my_modules[i].get_nti(); i_m++) {
					if (my_modules[i].message_input_array[i_m].name == "СС") {
						//if (my_modules[i].get_machine() == my_machine) {
							pairs.push_back(make_pair(&datas[pairs.size() - 1][0], &datas[pairs.size() - 1][0]));
							ss_channels.push_back(pairs.size() - 1);
							cout << "did" << pairs.size() - 1 << endl;
							my_modules[i].message_input_array[i_m].connection_type = 0;
							my_modules[i].message_input_array[i_m].channel_from = pairs.size() - 1;
							p = 8;
						//}
					}
				}
			}
		}
		vector<pthread_t> thids;
		starttime = timestamp();
		pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
		//pthread_attr_setschedparam (&attr, &param);

		if (pthread_create(&ss_thread, &attr, ss_module, (void *) NULL)) {
			handle_error("Error on ss_thread create");
		}
		//after SS thread we start others threads
		for(int i = 0; i < my_modules.size(); i++) {
			thids.push_back(thread);
            if( my_modules[i].get_affectation()) {
                cpu_id++;
                CPU_ZERO(&cpus);
        		CPU_SET(cpu_id, &cpus);
                pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
			    if (pthread_create(&thids[i], &attr, module, &my_modules[i])) {
				    perror("Error on thread create");
			    }                
            }
			if (pthread_create(&thids[i], (pthread_attr_t *) NULL, module, &my_modules[i])) {
				perror("Error on thread create");
			}
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
            if(my_modules[i].get_number() < 8) {
		        if(count_rec != 0) {
			            cout << my_modules[i].get_name() << " получил сообщений " << count_rec
			            << " цепочек " << count_rec / my_modules[i].get_nti() << endl;
		            if(count_send != 0)
			            cout << my_modules[i].get_name() << " отправил сообщений " << count_send
			            << " цепочек " << count_send / my_modules[i].get_nto() << endl;
		            cout << "Mаксимальная длина очереди: " << array_of_max_queue[my_modules[i].get_number()] << endl;
		       }
		   }
		   else {
		       if(my_modules[i].get_name() == "РЕГ")  {
                    cout << my_modules[i].get_name() << " получил сообщений " << count_rec
                    << " цепочек " << count_rec / 2 << endl;	
                    cout << "Mаксимальная длина очереди: " << array_of_max_queue[my_modules[i].get_number()] << endl;	
                }
                else  {
                        cout << my_modules[i].get_name() << " получил сообщений " << count_rec
                    << " цепочек " << count_rec << endl;     
                    cout << "Mаксимальная длина очереди: " << array_of_max_queue[my_modules[i].get_number()] << endl;        
		        }
		    }
		}

		for(int i = 0; i < my_modules.size(); i++) {
			cout << modules[i].get_name() << endl;
			int k = 0;
			while(array_of_queue[my_modules[i].get_number()][k] != 300)
			{   			   
				    cout << array_of_queue[my_modules[i].get_number()][k] << " ";
				    k++;
				    cout << array_of_queue[my_modules[i].get_number()][k] << endl;
				    k++;
			}
            cout << k/2 << endl;
			cout << endl;
		}
//*/
		cout << "finished" << endl;

	} else {
		cerr << "Wrong number of arguments. Input the number of your machine.";
		return 1;
	}
	return 0;
}

void * module (void * arg) {
    //uint64_t delay = timestamp() - starttime;
	Module * vals = (Module *) arg;
//	int text-right;
//	int number_of_current_pair_out;
	double counter = 0.5;
	double current = vals->get_data_amount();
	short messages = 1;
	int index = 0;
	int recv_index = 0; //for messages queue
	//int send_index = 0;
	int max_long_of_messages_queue = 0;
	int long_of_messages_queue = 0;
	long long int result = 1;
	string name = vals->get_name();
	vector<Module::message_input> m_i = vals->get_all_message_input();
	vector<Module::message_output> m_o = vals->get_all_message_output();
	int i = 0, m = 0, l = 0, k = 0, n = 0;

	int mess_by_param = 0;
	
	receiver *recv_object, *recv_object_q, *recv_object_s;
	recv_object_q = new receiver_queue;
	recv_object_s = new receiver_socket;
	sender *send_object, *send_object_q, *send_object_s;
	send_object_q = new sender_queue;
	send_object_s = new sender_socket;
	//uint64_t delay = timestamp() - starttime;
	int received = 0;


	while( (timestamp() - starttime) < TIME) {

		for (vector<Module::message_input>::iterator it = m_i.begin(); it != m_i.end(); ++it) {

			if (!it->connection_type)
				recv_object = recv_object_q;
			else
				recv_object = recv_object_s;
			bool flag = recv_object->wait_for_message(it->channel_from);
			//check if there any message. If no, switch thread
			if (flag) {

					if (!vals->get_affectation()) //if there no affectation
						usleep(0);
			}

			else {

				long_of_messages_queue = 0;
				while (recv_object->there_message(it->channel_from)) {
					//received++;
					//cout << "received mess N " << received << endl;
					long_of_messages_queue++;
					if (max_long_of_messages_queue < long_of_messages_queue)
						max_long_of_messages_queue = long_of_messages_queue;

					//receiving
					array_for_file[vals->get_number()][index] = 2; //bad
					//cout << index << endl;
					index++;
                    wait_n_microsec(it->time_hand);
/*
					for (l = 0; l < it->time_hand; l++) {
						result = 1;
						for (k = 1; k <= 250; k++) {
							result = result * k;
						}
					}
*/
					if (it->parameter)
						mess_by_param++;
				}
				if(long_of_messages_queue > 1) {
			    	array_of_queue[vals->get_number()][recv_index] = long_of_messages_queue;
			    	recv_index++;
			        array_of_queue[vals->get_number()][recv_index] = timestamp();
			    	recv_index++;
			    }
			}
		}
				//cout << vals->get_name() << " received from " << it->name_from << endl;
				//sending


		for (m = 0; m < mess_by_param; m++) {
			if (vals->get_data_amount() != 1) {
				if (current < counter) {
					messages = 0;
				}
				else {
					messages = 1;
					counter += 1.0;
				}
				current += vals->get_data_amount();
			}
			for (i = 0; i < messages; i++) {
				for (k = 0; k < m_o.size(); k++) {
                    wait_n_microsec(m_o[k].time_form);
/*
					for (l = 0; l < m_o[k].time_form; l++) {
						result = 1;
						for (n = 1; n <= 250; n++) {
							result = result * n;
						}
					}
*/
					if (!m_o[k].connection_type) { // type = queue
						send_object = send_object_q;
					}
					else { // type = socket
						send_object = send_object_s;
					}
                    send_object->send_message(m_o[k].channel_to);
					//if(send_object->send_message(m_o[k].channel_to) == -1) {
						//cout << vals->get_name() << endl;
					//}

					array_for_file[vals->get_number()][index] = 1;
					//cout << index << endl;
					index++;
					//cout << vals->get_name() << " sent to " << m_o[k].name_to << endl;
				}
			}
		}
		mess_by_param = 0;
	}

	//cout << recv_index << '\t' << send_index << endl;
	//if(!vals->get_affectation()) //if there no affectation
		sleep(1);

	//close sockets for receiving
	for(vector<Module::message_input>::iterator it1 = m_i.begin(); it1 != m_i.end(); ++it1 ) {
		if (it1->connection_type) {
			close(it1->channel_from);
		}
	}
	delete(send_object_q);
	delete(send_object_s);
	delete(recv_object_q);
	delete(recv_object_s);
	array_of_max_queue[vals->get_number()] = max_long_of_messages_queue;
	array_of_queue[vals->get_number()][recv_index] = 300;
	//cout << vals->get_name() << " finished " << endl;
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

map<int, string> parser3() {
	map<int, string> machine_address;
	char * cstr = new char [s3.length()+1];
	strcpy(cstr, s3.c_str());
	ifstream fin(cstr);
	char buff[SIZE];
	string address;
	int machine = 0;
	while (fin >> buff) {
		fin >> buff;
		machine = atoi(buff);
		fin >> buff;
		address = buff;
		machine_address[machine] = address;
	}
	return machine_address;

};

map<string, int> parser2() {
	map<string, int> module_machine;
	char * cstr = new char [s2.length()+1];
	strcpy(cstr, s2.c_str());
	ifstream fin(cstr);
	char buff[SIZE];
	string module;
	int machine;
	while (fin >> buff) {
		module = buff;
		fin >> machine;
		module_machine[module] = machine;
	}
	return module_machine;
}


vector<Module> parser1() {
	char * cstr = new char [s1.length()+1];
	strcpy(cstr, s1.c_str());
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

			vals[i].set_port(0);
			//bool aff;
			//fin >> aff;
			vals[i].set_affectation(0);

			double amount;
			fin >> amount;
			vals[i].set_data_amount(amount);
		}
		fin >> buff;
		if (strcmp(buff, "-") != 0) {

			Module::message_input m_i;

			m_i.name = buff;

			int time_hand;
			fin >> time_hand;
			m_i.time_hand = time_hand;

			bool parameter;
			fin >> parameter;
			m_i.parameter = parameter;

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

			vals[i].set_message_output(m_o, vals[i].get_nto());
			vals[i].inc_nto();
		}
		i++;
	}

	fin.close();
	return vals;
}
