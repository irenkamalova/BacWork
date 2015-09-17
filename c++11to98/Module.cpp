/*
 * Module.cpp
 *
 *  Created on: Apr 16, 2015
 *      Author: irisha
 */

#include "Module.h"

Module::Module() {
	// TODO Auto-generated destructor stub
}

Module::~Module() {
	// TODO Auto-generated destructor stub
}

void Module::set_name(string name) {
	this->name = name;
}

string Module::get_name() {
	return name;
}

void Module::set_dti(string dti, int i) {
	data_type_input[i] = dti;
}

string Module::get_dti(int i) {
	return data_type_input[i];
}

void Module::set_dto(string dto, int i) {
	data_type_output[i] = dto;
}

string Module::get_dto(int i) {
	return data_type_output[i];
}

void Module::set_data_amount(double data_amount) {
	this->data_amount = data_amount;
}

double Module::get_data_amount() {
	return data_amount;
}

void Module::set_th(int th, int i) {
	time_hand[i] = th;
}

int Module::get_th(int i) {
	return time_hand[i];
}

void Module::set_tf(int tf, int i) {
	time_form[i] = tf;
}

int Module::get_tf(int i) {
	return time_form[i];
}

void Module::set_number(int i) {
	number = i;
}

int Module::get_number() {
	return number;
}

void Module::inc_nti() {
	number_of_type_input++;
}

void Module::inc_nto() {
	number_of_type_output++;
}

void Module::set_nti(int i) {
    number_of_type_input = i;
}

int Module::get_nti() {
	return number_of_type_input;
}

void Module::set_nto(int i) {
    number_of_type_output = i;
}

int Module::get_nto() {
	return number_of_type_output;
}
void Module::set_npi(int npi) {
	this->npi = npi;
}
void Module::set_npo(int npo) {
	this->npo = npo;
}
int Module::get_npi() {
	return npi;
}
int Module::get_npo() {
	return npo;
}
void Module::set_nsopi_el(int element, int index) {
	nsopi[index] = element;
}
void Module::set_nsopo_el(int element, int index) {
	nsopo[index] = element;
}
int Module::get_nsopi_el(int index) {
	return nsopi[index];
}
int Module::get_nsopo_el(int index) {
	return nsopo[index];
}
void Module::inc_npi() {
	this->npi++;
}
void Module::inc_npo() {
	this->npo++;
}

void Module::set_port(int port) {
	this->port = port;
}

int Module::get_port() {
	return port;
}

void Module::set_socket_for_send(int index, int socket_for_send) {
	sockets_for_send[index] = socket_for_send;
}

int Module::get_socket_for_send(int index) {
	return sockets_for_send[index];
}

void Module::set_receiver(int element, int index) {
	receivers[index] = element;
}

int Module::get_receiver(int index) {
	return receivers[index];
}

void Module::set_index_cond_vars(int index_cond_var, int index) {
	index_cond_vars[index] = index_cond_var;
}

int Module::get_index_cond_vars(int index) {
	return index_cond_vars[index];
}

void Module::set_index_for_file(int index) {
	index_for_file = index;
}

int Module::get_index_for_file() {
	return index_for_file;
}

void Module::set_time_for_sleep(int ts) {
	time_for_sleep = ts;
}

int Module::get_time_for_sleep() {
	return time_for_sleep;
}

void Module::set_n_of_s(int number) {
	number_of_sockets = number;
}

int Module::get_n_of_s() {
	return number_of_sockets;
}

void Module::inc_n_of_s() {
	number_of_sockets++;
}

void Module::set_n_of_s_i(int number) {
	number_of_sockets_input = number;
}

int Module::get_n_of_s_i() {
	return number_of_sockets_input;
}

void Module::inc_n_of_s_i() {
	number_of_sockets_input++;
}

void Module::set_par(int par, int i) {
    parametrs[i] = par; 
}

int Module::get_par(int i) {
    return parametrs[i];
}
