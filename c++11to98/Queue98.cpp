#include "Queue.h"
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <time.h> 	 //clock_gettime()
#include <unistd.h> //sleep
#include <chrono>
#include <set>

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
    nopSS = 0; //number_of_pairs_for_SS = 0
	for(i = 0; i < arg.size(); i++) {
		arg[i].set_index_for_file(i);
	}
	set_index_for_file(i);
	this->general = arg;
}

void write_to_file(vector<Module> vals);

struct thread_data {
    Queue *runner;
    Module *arg;
};

struct thread_data_s {
    Queue *runner;
    vector<Module> vals;
};


void *module(void *threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    Queue *runner = my_data->runner;
    Module *arg = my_data->arg;
	runner->module_queue(arg);
}
void *modules(void *threadarg) {
    struct thread_data_s *my_data;
    my_data = (struct thread_data_s *) threadarg;
    Queue *runner = my_data->runner;
    vector<Module> vals = my_data->vals;
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
					vals[i].inc_npo();
					vals[j].set_nsopi_el(pairs.size() - 1, vals[j].get_npi());
					vals[j].inc_npi();
				}
			}
		}
		//Create pairs for general modules
		for(int j = 0; j < gen_size; j++) {
			for(int k = 0; k < general[j].get_nti(); k++) {
				if(vals[i].get_dto(0) == general[j].get_dti(k)) {
					pairs.push_back(make_pair(&datas[pairs.size() - 1][0], &datas[pairs.size() - 1][0]));
					vals[i].set_nsopo_el(pairs.size() - 1, vals[i].get_npo());
					vals[i].inc_npo();
					general[j].set_nsopi_el(pairs.size() - 1, general[j].get_npi());
					general[j].inc_npi();
				}
			}
		}
	}
	//Create pairs for SS module
	for(int i = 0; i < size; i++ ) {
	    for(int k = 0; k < vals[i].get_nti(); k++) {
	        if(vals[i].get_dti(k) == "СС") {
	        pairs.push_back(make_pair(&datas[pairs.size() - 1][0], &datas[pairs.size() - 1][0]));
	        vals[i].set_nsopi_el(pairs.size() - 1, vals[i].get_npi());
	        vals[i].inc_npi();
	        SS_nop[nopSS] = pairs.size() - 1;
	        nopSS++;
	        //SS number of this pair
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
	vector<pthread_t> thids;
    struct thread_data thrdarray[20];
	for(int i = 0; i < size; i++) {
	        thrdarray[i].runner = this;
	        thrdarray[i].arg = &vals[i];
			if (pthread_create(&thids[i], (pthread_attr_t *) NULL, module,
					&thrdarray[i])) {
				cerr << "Error on thread create!\n";
				exit(EXIT_FAILURE);
			}
	}
	//sleep(2);
	//cout << num_obj << " join\n";

	for (vector<pthread_t>::iterator it = thids.begin(); it != thids.end();
			++it) {
		pthread_join(*it, (void **) NULL);
	}
	//cout << num_obj << " afterjoin\n";
}

int Queue::run(int flows_auto, int flows_search) {
	//dump(general, "general", 0);
	cout << "Queue run\n";
    struct thread_data_s thrdarrays[200];
	struct thread_data thrdarray[20];
	/*/print numbers of pairs:
	for(auto& tag : SS_nop) {
	    cout << tag << endl;
	}*/
	//Preparation

	for(int i = 0; i < 203; i++) {
		for(int k = 0; k < 200000; k++)
			array_for_file[i][k] = 0;
	}
	vector<pthread_t> generals_threads(3);
	vector<pthread_t> thids(100);

	starttime = timestamp();	
	for(int i = 0; i < general.size(); i++ ) {
	        thrdarray[i].runner = this;
	        thrdarray[i].arg = &general[i];
			if (pthread_create(&generals_threads[i], (pthread_attr_t *) NULL, module,
					(void *)&thrdarray[i])) {
				cerr << "Error on thread create!\n";
				exit(EXIT_FAILURE);
			}
	}
	int k = 1;
	for(int i = 0; i < flows_auto; i++) {
	        thrdarrays[i].runner = this;
	        thrdarrays[i].vals = auto_accomp[i];
			if (pthread_create(&thids[k - 1], (pthread_attr_t *) NULL, modules,
					(void *)&thrdarrays[i])) {
				cerr << "Error on thread create!\n";
				exit(EXIT_FAILURE);
			}
			k++;
	}
	    cout << "No Error acc " << endl;
	for(int i = 0; i < flows_search; i++) {
	        thrdarrays[i].runner = this;
	        thrdarrays[i].vals = search[i];
			if (pthread_create(&thids[k - 1], (pthread_attr_t *) NULL, modules,
					(void *)&thrdarrays[i])) {
				cerr << "Error on thread create!\n";
				exit(EXIT_FAILURE);
			}
		k++;
	}
	//sleep(2);
	//cout << "join_general\n";
	
	//Start here SyncSignal (SS) module:
	index_for_file++;
	int sleep_time = 1000;
	int index = 0;
	int propusk = 0;
	//starttime = timestamp();
	long long int t_i = (long long int) starttime;
	if((long long int)(timestamp() - starttime) < 0) {
	    cout << "No Error " << endl;
	    return 0;
	}
cout << "No Error 0 " << endl;
	while((long long int)(timestamp() - starttime) < 10000000000) {
		if((long long int)(timestamp() - starttime) < 0) {
	        cout << "Error 187 " << endl;
	        return 0;
	    }
cout << "No Error 1 " << endl;
		int numeric_of_pair_for_output = nopSS;
		for(int i = 0; i < numeric_of_pair_for_output; i++) {
			int number_of_current_pair = SS_nop[i];
cout << "No Error 1.3 " << endl;
			send_message(number_of_current_pair);

cout << "No Error 1.6 " << endl;
			array_for_file[index_for_file][index++] = 1;
			//array_for_file[vals->get_index_for_file()][index++] = (long long int)(timestamp() - starttime);
		}      
cout << "No Error 2 " << endl;
		t_i = t_i + sleep_time * 1000;
		while( (t_i - (long long int)timestamp()) < 0 ) {
		    t_i = t_i + sleep_time * 1000;
		    propusk++;
		    }
cout << "No Error 3 " << endl;
		//cout << (t_i - (long long int)timestamp()) / 1000 << endl;
		if(t_i - (long long int)timestamp() < 0) {
		    cout << "Error 205" << endl;
		    return 0;
		}
        int for_usleep = (t_i - (long long int)timestamp()) / 1000;
cout << "No Error 4 " << endl;
cout << for_usleep << endl;
		usleep( 2 );
cout << "No Error 5 " << endl;
	}	
	cout << "AFTER SS END WORK" << endl;

	for (vector<pthread_t>::iterator it =  generals_threads.begin(); it !=  generals_threads.end();
			++it) {
		pthread_join(*it, (void **) NULL);
	}	
	cout << "after_join_general\n";
	cout << "join_thids\n";
	for (vector<pthread_t>::iterator it = thids.begin(); it != thids.end();
			++it) {
		pthread_join(*it, (void **) NULL);
	}
	cout << "after_join_all\n";
	k = 0;
	write_to_file(general, k);
	cout << "general_write\n";
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

	return 0;
}

void Queue::module_queue(Module *vals) {
    int i = 0;
    /*Check
    string file_name = "./check_modules/" + vals->get_name() + ".txt";
    ofstream fout(file_name);
    fout << "Name:\n" << vals->get_name() << "\nInput data:\n" << "Number: " << vals->get_npi() << endl;
    for(int i = 0; i < vals->get_npi(); i++)
        fout << vals->get_dti(i) << " " << vals->get_th(i) << endl;
    if(vals->get_npo() != 0) {
        fout << "Main input data:\n";
        for(int i = 0; i < vals->get_npi(); i++)
            if(vals->get_par(i) == 1) 
                fout << vals->get_dti(i) << endl;
        fout << "Output data:\n";
        for(int i = 0; i < vals->get_npo(); i++) 
            fout << vals->get_dto(i) << " " << vals->get_tf(i) << endl;
    }
    fout.close();
    */
	int count = 0;
	bool ifsend1 = false;
	double counter = 0.5;
	double current = vals->get_data_amount();

	bool flag_mes_received = false;
	int index = 0;
	long long int t_i = (long long int) starttime;	
	int numeric_of_pair_for_input = vals->get_npi();
    //cout << vals->get_name() << vals->get_par(0) << endl;	
    while( (long long int)(timestamp() - starttime) < 10000000000) {
	    if((long long int)(timestamp() - starttime) < 0) {
	        cout << "Error 270 " << endl;
            cout << vals->get_name() << endl;
	    }    
	//cout << vals->get_name() << (long long int)(timestamp() - starttime) << endl;
	
	for(int i = 0; i < numeric_of_pair_for_input; i++) {
		int number_of_current_pair = vals->get_nsopi_el(i);
		
		if(pairs[number_of_current_pair].first == pairs[number_of_current_pair].second) {
			usleep(vals->get_time_for_sleep());
		}		
	    else while(pairs[number_of_current_pair].first != pairs[number_of_current_pair].second) {
	        if(vals->get_par(i) == 1)
	            flag_mes_received = true;
			receive_message(number_of_current_pair);

			array_for_file[vals->get_index_for_file()][index++] = 2;

			for(int l = 0; l < vals->get_th(i); l++) {
				long long int result = 1;
				for (int k = 1; k <= 250; k++) {
					result = result * k;
				}
			}
		}
	}
	if(flag_mes_received) {
	    if(vals->get_data_amount() != 1) {
            if(current < counter) {
                ifsend1 = false;
            }
            else {
                ifsend1 = true;
                counter += 1.0;
            }
            current += vals->get_data_amount();
            if(!ifsend1) {
                flag_mes_received = false;
                usleep(vals->get_time_for_sleep());
            }
        }
        else ifsend1 = true;
	    if(ifsend1) {
	        int numeric_of_pair_for_output = vals->get_npo();
	        for(int i = 0; i < numeric_of_pair_for_output; i++) {
		        int number_of_current_pair = vals->get_nsopo_el(i);
		        for(int l = 0; l < vals->get_tf(i); l++) {
			        long long int result = 1;
			        for (int k = 1; k <= 250; k++) {
				        result = result * k;
			        }
		        }
		        send_message(number_of_current_pair);
		        array_for_file[vals->get_index_for_file()][index++] = 1;
		        //array_for_file[vals->get_index_for_file()][index++] = (long long int)(timestamp() - starttime);
	        } 
	        ifsend1 = false;
	    }
	    flag_mes_received = false;
    }
	    
    }
    //cout << vals->get_name() << "endhiswork" << endl;
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

	//string s = "./queue/result_queue" + num_obj + ".txt";
	string s2 = "./queue/messages_queue" + num_obj + ".txt";
	//ofstream fout(s);
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
			//fout << vals[i].get_name();
			if(array_for_file[vals[i].get_index_for_file()][k] == 1) {
				//fout << " отправил в ";
				k++;
				count_send++;
			}
			else if(array_for_file[vals[i].get_index_for_file()][k] == 2) {
				//fout << " получил в ";
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
			}
			/*
			fout << seconds << " секунд " <<
					mini_seconds << " милисекунд " <<
					micro_seconds << " микросекунд " <<
					nano_seconds << " наносекунд "
					<< endl;
			k++;
            */
		}
		//fout << endl;
		
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
