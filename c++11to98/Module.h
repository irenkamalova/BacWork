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
	Module();
	virtual ~Module();

	void set_name(string name);
	string get_name();

	void set_number(int number);
	int get_number();

	void set_dti(string dti, int i);
	string get_dti(int i);

	void set_dto(string dti, int i);
	string get_dto(int i);

	void set_data_amount(double data_amaunt);
	double get_data_amount();

	void set_th(int th, int i);
	int get_th(int i);

	void set_tf(int tf, int i);
	int get_tf(int i);

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

	void set_index_cond_vars(int index_cond_var, int index);
	int get_index_cond_vars(int index);

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
	string data_type_input[SIZE];
	string data_type_output[SIZE];
	double data_amount;
	int time_hand[SIZE];
	int time_form[SIZE];
	int number_of_type_input;
	int number_of_type_output;
	int receivers[SIZE];
	int number;
	int time_for_sleep;

	//for queue
	int nsopi[20]; //numbers_of_pairs_input
	int nsopo[20]; //numbers_of_pairs_output
	int npi;//number of pairs for input
	int npo;//number of pairs for output
	int index_my_cond_var;
	int index_cond_vars[SIZE]; // indexes for conditions variables that have to notify after sending message
	int index_for_file;

	//for sockets
	int port;
	int sockets_for_send[SIZE];
	int number_of_sockets;
	int number_of_sockets_input;
};

#endif /* MODULE_H_ */
