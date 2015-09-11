/*
 * Queue.cpp
 *
 *  Created on: Apr 22, 2015
 *      Author: irisha
 */

#include "Queue.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <time.h> 	 //clock_gettime()
#include <unistd.h> //sleep
//#include <errno.h>
#include <chrono>

using namespace std;


Queue::Queue() {
}

Queue::~Queue() {
}

void Queue::set_starttime() {
	starttime = timestamp();
}

uint64_t Queue::get_starttime() {
	return starttime;
}

uint64_t Queue::timestamp() {
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (static_cast<uint64_t>(ts.tv_sec) * 10000000u
			+ static_cast<uint64_t>(ts.tv_nsec) / 100u) * 100u;
}

void Queue::set_general(vector<Module> arg) {
	int i;
	for(i = 0; i < arg.size(); i++) {
		arg[i].set_index_for_file(i);
	}
	set_index_for_file(i);
	this->general = arg;
}

void write_to_file(vector<Module> vals);

void module(Queue *runner, Module *arg) {
	runner->module_queue(arg);
}
void modules(Queue *runner, vector<Module> vals) {
	runner->modules_queue(vals);
}

void Queue::set_index_for_file(int index) {
	index_for_file = index;
}

void Queue::create_pairs(vector<Module> vals, int num_object, modules_types m_t) {
	int size = vals.size();
	int gen_size = general.size();
	for(int i = 0; i < size; i++) {
		vals[i].set_npo(0);
		vals[i].set_npi(0);
		vals[i].set_index_for_file(index_for_file);
		index_for_file++;
	}
	for(int i = 0; i < size; i++ ) {
		for(int j = i + 1; j < size; j++) {
			for(int k = 0; k < vals[j].get_nti(); k++) {
				if(vals[i].get_dto(0) == vals[j].get_dti(k)) {
					pairs.push_back(make_pair(&datas[pairs.size() - 1][0], &datas[pairs.size() - 1][0]));
					vals[i].set_nsopo_el(pairs.size() - 1, vals[i].get_npo());
					//vals[i].set_index_cond_vars(vals[j].get_index_cond_var(), vals[i].get_npo());
					vals[i].inc_npo();
					vals[j].set_nsopi_el(pairs.size() - 1, vals[j].get_npi());
					vals[j].inc_npi();
				}
			}
		}
		for(int j = 0; j < gen_size; j++) {
			for(int k = 0; k < general[j].get_nti(); k++) {
				if(vals[i].get_dto(0) == general[j].get_dti(k)) {
					pairs.push_back(make_pair(&datas[pairs.size() - 1][0], &datas[pairs.size() - 1][0]));
					//cout << pairs.size() << " " << pairs[pairs.size() - 1].first << " " << pairs[pairs.size() - 1].second << endl;
					vals[i].set_nsopo_el(pairs.size() - 1, vals[i].get_npo());
					//vals[i].set_index_cond_vars(general[j].get_index_cond_var(), vals[i].get_npo());
					vals[i].inc_npo();
					general[j].set_nsopi_el(pairs.size() - 1, general[j].get_npi());
					//cout << general[j].get_nsopi_el(general[j].get_npi())<< " " << general[j].get_npi() << endl;
					general[j].inc_npi();
				}
			}
		}
	}
	//dump(vals, s, num_object);
	if(m_t == AUTO) {
		auto_accomp.push_back(vals);
	}
	else if(m_t == SEARCH) {
		search.push_back(vals);
	}
}

void Queue::modules_queue(vector<Module> vals) {
	int size = vals.size();
	vector<thread> thids;
	for(int i = 0; i < size; i++) {
		thids.push_back(thread(module, this, &vals[i]));
	}
	//sleep(2);
	//cout << num_obj << " join\n";
	for (auto& thr : thids) {
		thr.join();
	}
	//cout << num_obj << " afterjoin\n";
}

int Queue::run(int flows_auto, int flows_search) {
	//dump(general, "general", 0);
	cout << "Queue run\n";
	for(int i = 0; i < 203; i++) {
		for(int k = 0; k < 200000; k++)
			array_for_file[i][k] = 0;
	}
	vector<thread> generals_threads;
	vector<thread> thids;

	starttime = timestamp();	
	for(int i = 0; i < general.size(); i++ ) {
			generals_threads.push_back(thread(module, this, &general[i]));
	}
	int k = 1;
	for(int i = 0; i < flows_auto; i++) {
		thids.push_back(thread(modules, this, auto_accomp[i]));
		k++;
	}

	for(int i = 0; i < flows_search; i++) {
		thids.push_back(thread(modules, this, search[i]));
		k++;
	}
	//sleep(2);
	//cout << "join_general\n";
	for (auto& thr : generals_threads) {
		thr.join();
	}
	//cout << "after_join_general\n";
	//cout << "join_thids\n";
	for (auto& thr : thids) {
		thr.join();
	}
	//cout << "after_join_all\n";
	k = 0;
	write_to_file(general, k);
	//cout << "general_write\n";
	k++;
	for(int i = 0; i < flows_auto; i++ ) {
		vector<Module> like = auto_accomp[i];
		write_to_file(auto_accomp[i], k);
		k++;
	}
	for(int i = 0; i < flows_search; i++ ) {
		write_to_file(search[i], k);
		k++;
	}
	//cout << "after_delete\n";
	return 0;
}

void Queue::module_queue(Module *vals) {


	int count = 0;
	bool ifsend1 = false;
	double counter = 0.5;
	double current = vals->get_data_amount();
	int count_mess = 0;

	bool flag_mes_received = false;
	int index = 0;
	long long int t_i = (long long int) starttime;	
	int propusk = 0;
	//cout << vals->get_name() << endl;
	//while( (long long int)(timestamp() - st_time) < 10000000000) {
		//cout << (long long int)(timestamp() - starttime) << endl;
        
		if((vals->get_name() == "ФВД_АС" ) || (vals->get_name() == "ФВД_П" )) {
			while((long long int)(timestamp() - starttime) < 10000000000) {

				while(!ifsend1) {
					if(current < counter) {
						ifsend1 = false;
					}
					else {
						ifsend1 = true;
						counter += 1.0;
					}
					current += vals->get_data_amount();
					if(!ifsend1) {

						usleep(vals->get_time_for_sleep());
					}
				}
				
				for(int l = 0; l < vals->get_tf(0); l++) {
					long long int result = 1;
					for (int k = 1; k <= 250; k++) {
						result = result * k;
					}
				}
				int numeric_of_pair_for_output = vals->get_npo();
				for(int i = 0; i < numeric_of_pair_for_output; i++) {
					int number_of_current_pair = vals->get_nsopo_el(i);
					send_message(number_of_current_pair);
					array_for_file[vals->get_index_for_file()][index++] = 1;
					//array_for_file[vals->get_index_for_file()][index++] = (long long int)(timestamp() - starttime);
				}
				//cout << vals->get_time_for_sleep() << endl;
				//cout << (int) (timestamp() - delay) / 1000 << endl;
				//cout << vals->get_time_for_sleep() - (int) ((timestamp() - delay) / 1000) << endl;
				//int time_delay = (int) ((timestamp() - delay) / 1000);
				//cout << time_delay << endl;
				t_i = t_i + vals->get_time_for_sleep() * 1000;
				while( (t_i - (long long int)timestamp()) < 0 ) {
				    t_i = t_i + vals->get_time_for_sleep() * 1000;
				    propusk++;
				    }
				//cout << (t_i - (long long int)timestamp()) / 1000 << endl;
				usleep( (t_i - (long long int)timestamp()) / 1000 );
				ifsend1 = false;
			}
		}
		else {
		    while( (long long int)(timestamp() - starttime) < 10000000000) {
			//cout << vals->get_name() << (long long int)(timestamp() - starttime) << endl;
			int numeric_of_pair_for_input = vals->get_npi();
			for(int i = 0; i < numeric_of_pair_for_input; i++) {
				int number_of_current_pair = vals->get_nsopi_el(i);

				//cout << i << " " << pairs[number_of_current_pair].first << " " << pairs[number_of_current_pair].second << endl;
				//cout << vals->get_name() << endl;
				if(pairs[number_of_current_pair].first == pairs[number_of_current_pair].second) {
					//cout << vals->get_name() << " before sleep" << endl;
					//cout << vals->get_time_for_sleep() << endl;
					usleep(vals->get_time_for_sleep());
				}
				//cout << vals->get_name() << endl;
				while(pairs[number_of_current_pair].first != pairs[number_of_current_pair].second) {
					receive_message(number_of_current_pair);
					count_mess++;
					//cout << vals->get_name() << pairs[number_of_current_pair].second << endl;
					array_for_file[vals->get_index_for_file()][index++] = 2;
					//array_for_file[vals->get_index_for_file()][index++] = (long long int)(timestamp() - starttime);

					if (vals->get_npo() != 0) {
						if(count_mess == numeric_of_pair_for_input)
							flag_mes_received = true;
					}
					for(int l = 0; l < vals->get_th(i); l++) {
						long long int result = 1;
						for (int k = 1; k <= 250; k++) {
							result = result * k;
						}
					}
				}
			}
			if(flag_mes_received) {
				int numeric_of_pair_for_output = vals->get_npo();
				for(int i = 0; i < numeric_of_pair_for_output; i++) {
					int number_of_current_pair = vals->get_nsopo_el(i);
					for(int l = 0; l < vals->get_tf(i); l++) {
						long long int result = 1;
						for (int k = 1; k <= 250; k++) {
							result = result * k;
						}
					}
					//cout << "!!!!!!!" << vals->get_name()
					send_message(number_of_current_pair);
					array_for_file[vals->get_index_for_file()][index++] = 1;
					//array_for_file[vals->get_index_for_file()][index++] = (long long int)(timestamp() - starttime);
				}
				flag_mes_received = false;
				count_mess = 0;
			}
		}
	}
	//if(vals->get_name() == "ФВД_АС")
        //cout << propusk << endl;
}




void Queue::dump(vector<Module> vals, string s, int num_object) {
        char buffer[5];
        sprintf(buffer, "%d", num_object);
        string num_obj(buffer);

	string file_name = "./queue/dump_" + s + num_obj + ".txt";
	ofstream fout(file_name);
	int size = vals.size();
	for(int i = 0; i < size; i++) {
		fout << vals[i].get_name() << " " ;
		fout << "номера пар на получение сообщений: ";
		for(int k = 0; k < vals[i].get_npi(); k++) {
			fout << vals[i].get_nsopi_el(k) << " ";
		}
		fout << "номера пар на отправление сообщений: ";
		for(int k = 0; k < vals[i].get_npo(); k++) {
			fout << vals[i].get_nsopo_el(k) << " ";
		}
			fout << endl;
	}
}

void Queue::write_to_file(vector<Module> vals, int num_object) {

	cout << "in " << num_object << endl;
        char buffer[4];
        sprintf(buffer, "%d", num_object);
        string num_obj(buffer);

	string s = "./queue/result_queue" + num_obj + ".txt";
	string s2 = "./queue/messages_queue" + num_obj + ".txt";
	ofstream fout(s);
	ofstream fout2(s2);
	long long int time = 0;
	int nano_seconds = 0;
	int micro_seconds = 0;
	int mini_seconds = 0;
	int seconds = 0;
	int count_send = 0;
	int count_rec = 0;
	for(int i = 0; i < vals.size(); i++) {
		int k = 0;
		count_send = 0;
		count_rec = 0;
		while(array_for_file[vals[i].get_index_for_file()][k] != 0) {
			fout << vals[i].get_name();
			if(array_for_file[vals[i].get_index_for_file()][k] == 1) {
				fout << " отправил в ";
				k++;
				count_send++;
			}
			else if(array_for_file[vals[i].get_index_for_file()][k] == 2) {
				fout << " получил в ";
				k++;
				count_rec++;
			}
			time = array_for_file[vals[i].get_index_for_file()][k];
			seconds = time / 1000000000;
			if(seconds > 0) {
				mini_seconds = time / 1000000 - seconds * 1000;
				micro_seconds = time / 1000 - mini_seconds * 1000 - seconds * 1000000;
				nano_seconds = time - micro_seconds * 1000 - mini_seconds * 1000000 - seconds * 1000000000;
			} else {
				mini_seconds = time / 1000000;
				micro_seconds = time / 1000 - mini_seconds * 1000;
				nano_seconds = time - micro_seconds * 1000 - mini_seconds * 1000000;
			}/*
			fout << seconds << " секунд " <<
					mini_seconds << " милисекунд " <<
					micro_seconds << " микросекунд " <<
					nano_seconds << " наносекунд "
					<< endl;
			k++;*/

		}
		fout << endl;
		if(num_object != 0) {

			if(count_rec != 0)
				fout2 << vals[i].get_name() << " получил сообщений " << count_rec
				<< " цепочек " << count_rec / vals[i].get_npi() << endl;
			if(count_send != 0)
				fout2 << vals[i].get_name() << " отправил сообщений " << count_send
				<< " цепочек " << count_send / vals[i].get_npo() << endl;
		}
		else {
			if(vals[i].get_name() != "Рег")
				fout2 << vals[i].get_name() << " получил сообщений " << count_rec
				<< " цепочек " << count_rec << endl;
			else
				fout2 << vals[i].get_name() << " отправил сообщений " << count_rec
				<< " цепочек " << count_rec / 2 << endl;
		}
	}
}

void Queue::send_message(int number_of_current_pair) {
	if(pairs[number_of_current_pair].first != &datas[number_of_current_pair][LENGTH_OF_ARRAY]) {
		pairs[number_of_current_pair].first = pairs[number_of_current_pair].first + 1;
	}
	else
		pairs[number_of_current_pair].first = &datas[number_of_current_pair][0];
}

void Queue::receive_message(int number_of_current_pair) {
	
	if(pairs[number_of_current_pair].second != &datas[number_of_current_pair][LENGTH_OF_ARRAY]) {
		pairs[number_of_current_pair].second = pairs[number_of_current_pair].second + 1;
	}
	else
		pairs[number_of_current_pair].second = &datas[number_of_current_pair][0];
}