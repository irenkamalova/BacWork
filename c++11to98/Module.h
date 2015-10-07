/*
 * Module.h
 *
 *  Created on: Apr 16, 2015
 *      Author: irisha
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <string>
#include <vector>

const int SIZE = 100;
using namespace std;

class Module {
public:
	struct message_input {
		string name;
		string name_from;
		int time_hand;
		bool parameter;
		bool connection_type;
		int channel_from;
		int socket_from;

	};

	struct message_output {
		string name;
		string name_to;
		int time_form;
		bool connection_type;
		int channel_to;
		int socket_to;
	};

	Module();
	virtual ~Module();

	void set_name(string name);
	string get_name();

	void set_number(int number);
	int get_number();

	void set_data_amount(double data_amaunt);
	double get_data_amount();

	void set_message_input(message_input m_i, int index);
	message_input get_message_input(int index);
	vector<message_input> get_all_message_input();

	void set_message_output(message_output m_o, int index);
	message_output get_message_output(int index);
	vector<message_output> get_all_message_output();

	// control number of messages input ans output
	void set_nti(int i);
	void set_nto(int i);
	void inc_nti();
	void inc_nto();
	int get_nti();
	int get_nto();

	void set_time_for_sleep(int ts);
	int get_time_for_sleep();

	void set_index_for_file(int index);
	int get_index_for_file();


	//for sockets
	void set_port(int port);
	int get_port();

	void set_n_of_s(int number);
	int get_n_of_s();
	void inc_n_of_s();

	void set_socket_for_send(int index, int socket_for_send);
	int get_socket_for_send(int index);

	void set_n_of_s_i(int number);
	int get_n_of_s_i();
	void inc_n_of_s_i();



private:

	string name;
	double data_amount;
	int number_of_messages_input;
	int number_of_messages_output;
	int number;
	int time_for_sleep;

	//for both

	int index_for_file;

	//for sockets
	int port;
	int sockets_for_send[SIZE];
	int number_of_sockets;
	int number_of_sockets_input;

	vector<message_input> message_input_array;
	vector<message_output> message_output_array;
};

#endif /* MODULE_H_ */
