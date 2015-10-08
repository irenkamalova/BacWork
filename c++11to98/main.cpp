#include <iostream>
#include "Module.h"
#include "Queue.h"
#include "QueueAndSockets.h"
//#include "Socket.h"
#include <vector>
#include <cstring>
#include <fstream>
#include <time.h> 	 //clock_gettime()
#include <unistd.h>  //for sleep
#include <stdlib.h>  //atoi

vector<Module> parser(string s);
vector<Module> new_parser(string s);
void check_parser(vector<Module> vals, string s);

int main(int argc, char *argv[]) {
	const char* both_way = "queue_and_socket";
	
	if (argc == 4) {
        //vector<Module> general = parser("modules_general.txt");
        //vector<Module> a = parser("modules_auto.txt");
        //vector<Module> b = parser("modules_search.txt");
        //check_parser(a, "a.txt");
        //return 0;
        int flow1 = atoi(argv[2]);
        int flow2 = atoi(argv[3]);
		if (strcmp(argv[1], both_way) == 0) {
			vector<Module> modules = parser("/home/newuser/modules.txt");
			QueueAndSockets *queueAndSockets = new QueueAndSockets;
			queueAndSockets->run(modules);
		}

		else {
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
			int port;
			fin >> port;
			vals[i].set_port(port);

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
			}

			vals[i].set_message_output(m_o, vals[i].get_nto());
			vals[i].inc_nto();
		}
		i++;
	}

	fin.close();
	return vals;
}
