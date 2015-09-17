#include <iostream>
#include "Module.h"
#include "Queue.h"
#include "Socket.h"
#include <vector>
#include <cstring>
#include <fstream>
#include <thread>
#include <time.h> 	 //clock_gettime()
#include <unistd.h>  //for sleep

vector<Module> parser(string s);
void check_parser(vector<Module> vals, string s);

int main(int argc, char *argv[]) {
	const char* queue_way = "queue";
	const char* socket_way = "socket";
	
	if (argc == 4) {
        vector<Module> general = parser("modules_general.txt");
        check_parser(general, "check_general.txt");
        vector<Module> a = parser("modules_auto.txt");
        vector<Module> b = parser("modules_search.txt");
        //check_parser(a, "a.txt");
        //return 0;
        int flow1 = atoi(argv[2]);
        int flow2 = atoi(argv[3]);
		if (strcmp(argv[1], queue_way) == 0) {
			Queue *queue = new Queue();
			queue->set_general(general);
            queue->m_t = Queue::AUTO;
			for(int i = 0; i < flow1; i++) {
			    queue->create_pairs(a, i, queue->m_t);
			}
            queue->m_t = Queue::SEARCH;
			for(int i = 0; i < flow2; i++) {
			    queue->create_pairs(b, i, queue->m_t);
			}
			queue->run(flow1, flow2);
            delete queue;
		} else if (strcmp(argv[1], socket_way) == 0) {
			Socket *socket = new Socket();
			socket->set_general(general);
            socket->m_t = Socket::AUTO;
			for(int i = 0; i < flow1; i++) {
			    socket->create_ports(a, i, socket->m_t);
			}
            socket->m_t = Socket::SEARCH;
			for(int i = 0; i < flow2; i++) {
			    socket->create_ports(b, i, socket->m_t);
			}
			socket->run(flow1, flow2);
            delete socket;
		} else {
			cerr << "Wrong simulator name! Input queue or socket";
			return 1;
		}
	} else {
		cerr << "Wrong number of arguments. Input name of simulator and flows.";
		return 1;
	}
	return 0;
}

vector<Module> parser(string s) {

	ifstream fin(s);
	char buff[SIZE];
	int i = 0, l = 0;
	int k = 0, m = 0;
	vector<Module> vals;

	while (fin >> buff) {
		if (strcmp(buff, "-") == 0) {
			i = i - 1;
			k = k + 1;
		} else {
			Module *module = new Module;
			module->set_name(buff);
			vals.push_back(*module);
			k = 0;
			vals[i].set_number(i);
			vals[i].set_nti(0);
			vals[i].set_nto(0);
			int time;
			fin >> time;
			vals[i].set_time_for_sleep(time);
			double data_amount;
			fin >> data_amount;
			vals[i].set_data_amount(data_amount);
            delete module;
		}
		fin >> buff;
		if (strcmp(buff, "-") != 0) {
			vals[i].set_dti(buff, k);
			int parametr;
			fin >> parametr;
            
			int time_hand;
			fin >> time_hand;
			int number_of_data_type = vals[i].get_nti();
			vals[i].set_th(time_hand, number_of_data_type);
			vals[i].set_par(parametr, number_of_data_type);
			
			vals[i].inc_nti();
		} else
			fin >> buff;
		fin >> buff;
		if (strcmp(buff, "-") != 0) {
			vals[i].set_dto(buff, 0); //не предусматривает количество выходных сообщений > 1
			int time_form;
			fin >> time_form;
			int number_of_data_type = vals[i].get_nto();
			vals[i].set_tf(time_form, number_of_data_type);
			vals[i].inc_nto();
		} else
			fin >> buff;
		i++;
	}
	fin.close();
	
	return vals;
}

void check_parser(vector<Module> vals, string s) {
	ofstream fout(s);
	for(int i = 0; i < vals.size(); i++) {
		fout << vals[i].get_number() << " " << vals[i].get_name() << " " << vals[i].get_data_amount()
				<< "  " << vals[i].get_time_for_sleep() << " ";
		for(int k = 0; k < vals[i].get_nti(); k++) {
			fout << vals[i].get_dti(k) << " " << vals[i].get_th(k) << " ";
		}
		for(int k = 0; k < vals[i].get_nto(); k++) {
					fout << vals[i].get_dto(k) << " " << vals[i].get_tf(k) << " ";
		}
	fout << endl;
	}
	fout.close();
}
