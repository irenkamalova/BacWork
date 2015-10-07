/*
 * Queue.h
 *
 *  Created on: Apr 22, 2015
 *      Author: irisha
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <vector>
#include "Module.h"
#include <utility> //pairs // no (
#include <stdint.h> // use uint64_t type
#include <map>

class Queue {
public:

	Queue();
	virtual ~Queue();
    enum modules_types {AUTO, SEARCH} m_t;
	int run(int flows_auto, int flows_search);
	void create_pairs(vector<Module> vals, int num_object, modules_types m_t);
	void dump(vector<Module> vals, string s, int i);
	void send_message(int num);
	void receive_message(int num);
	void module_queue(Module *vals);
	void write_to_file(vector<Module> vals, int num_object);
	void set_starttime();
	uint64_t timestamp();
	uint64_t get_starttime();
	void set_general(vector<Module> arg);
	void modules_queue(vector<Module> vals);
	void set_index_cond_var(int index);
	void set_index_for_file(int index);

private:
	static const int LENGTH_OF_ARRAY = 50;
	static const long long int TIME = 10000000000;
	int datas[20][LENGTH_OF_ARRAY];
	vector< pair<int*, int*> > pairs;
	long long int array_for_file[203][200000];
	uint64_t starttime;
	vector<Module> general;
	vector< vector<Module> > auto_accomp;
	vector< vector<Module> > search;
	int index_for_file;
	int SS_nop[20];
	int nopSS;
	int sleep_time;

};
#endif /* QUEUE_H_ */
