/*
 * Module.cpp
 *
 *  Created on: Apr 16, 2015
 *      Author: irisha
 */

#include "Module.h"

Module::Module() {
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

void Module::set_data_amount(double data_amount) {
	this->data_amount = data_amount;
}
double Module::get_data_amount() {
	return data_amount;
}

void Module::set_number(int i) {
	number = i;
}
int Module::get_number() {
	return number;
}

void Module::set_message_input(message_input m_i, int index) {
	message_input_array.push_back(m_i);
}
Module::message_input Module::get_message_input(int index) {
	return message_input_array[index];
}

void Module::set_message_output(message_output m_o, int index) {
	message_output_array.push_back(m_o);
}
Module::message_output Module::get_message_output(int index) {
	return message_output_array[index];
}

void Module::set_nti(int i) {
	number_of_messages_input = i;
}
void Module::set_nto(int i) {
	number_of_messages_output = i;
}
void Module::inc_nti() {
	number_of_messages_input++;
}
void Module::inc_nto() {
	number_of_messages_output++;
}
int Module::get_nti() {
	return number_of_messages_input;
}
int Module::get_nto() {
	return number_of_messages_output;
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

vector<Module::message_input> Module::get_all_message_input() {
	return this->message_input_array;
}

vector<Module::message_output> Module::get_all_message_output() {
	return this->message_output_array;
}