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

vector<Module::message_input> Module::get_all_message_input() {
	return this->message_input_array;
}

vector<Module::message_output> Module::get_all_message_output() {
	return this->message_output_array;
}

vector<Module::message_input>* Module::get_address_of_messages_input() {
	return &message_input_array;
}

vector<Module::message_output>* Module::get_address_of_messages_output() {
	return &message_output_array;
}

void Module::set_machine(int machine) {
	this->machine = machine;
}

int Module::get_machine() {
	return this->machine;
}

void Module::set_my_ip_address(string ip_address) {
	this->my_ip_address = ip_address;
}

string Module::get_my_ip_address() {
	return this->my_ip_address;
}

void Module::set_affectation(bool aff) {
	this->affectation = aff;
}

bool Module::get_affectation() {
	return this->affectation;
}