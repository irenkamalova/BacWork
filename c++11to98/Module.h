/*
 * Module.h
 *
 *  Created on: Apr 16, 2015
 *      Author: irisha
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <string>
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

	};

	struct message_output {
		string name;
		string name_to;
		int time_form;
		bool connection_type;
		int channel_to;
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
	//message_input get_all_message_input();

	void set_message_output(message_output m_o, int index);
	message_output get_message_output(int index);
	//message_output get_all_message_output();

	// control number of messages input ans output
	void set_nti(int i);
	void set_nto(int i);
	void inc_nti();
	void inc_nto();
	int get_nti();
	int get_nto();



	void set_receiver(int element, int index);
	int get_receiver(int index);

	void set_time_for_sleep(int ts);
	int get_time_for_sleep();

	void set_index_for_file(int index);
	int get_index_for_file();

	//for both

	//for queue
	void set_npi(int npi);
	void set_npo(int npo);
	void inc_npi();
	void inc_npo();
	int get_npi();
	int get_npo();

	void set_nsopi_el(int element, int index);
	void set_nsopo_el(int element, int index);
	int get_nsopi_el(int index);
	int get_nsopo_el(int index);

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
	int receivers[SIZE];
	int number;
	int time_for_sleep;

	//for both


	//for queue
	int nsopi[20]; //numbers_of_pairs_input
	int nsopo[20]; //numbers_of_pairs_output

	int index_for_file;

	//for sockets
	int port;
	int sockets_for_send[SIZE];
	int number_of_sockets;
	int number_of_sockets_input;



	message_input message_input_array[SIZE];
	message_output message_output_array[SIZE];
};

#endif /* MODULE_H_ */
