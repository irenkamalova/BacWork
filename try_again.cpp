#include <cstring>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
////////////////////
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

#include <cstdlib>
#include <pthread.h>

#include <fcntl.h>

#include <time.h>
// Generic C++
#include <iostream>
#include <string>
#include <vector>
#include <map>

const int CONST = 100000;
const int SIZE = 100;
const int PORT = 60300;
const int NUMBER = 3;
const int TIME = 30;

using namespace std;

long long int output[100][300];
multimap<string, int> data_map;
//Arguments are needed for module
struct Arg {
	int number;
	int number_of_flows;
	char name[SIZE];
	char addition_name[SIZE];
	string data_type_input[SIZE];
	string data_type_output[SIZE];
	int time_hand[SIZE];
	int time_form[SIZE];
	int port1;
	int sock_send[SIZE];
	int sock_recv;
	double data_amount;
};

vector<Arg> parser();

//this function will execute each thread
void *module(void *arg);
void *thread_sync(void *arg);
int create_socket(int port);
int for_receiving(int port);
int writing(int size, int count, int i);
void write_result_into_file(vector<Arg> vals, int size);

void close_all_sockets(void *arg) {
	vector<Arg>* vals = (vector<Arg> *)arg;
	int size = vals->size();
	for(int i = 0; i < size; i++) {
		string out = (*vals)[i].data_type_output[0];
		if(out != "") {
			for(int k = 0; k < data_map.count(out); k++) {
				close((*vals)[i].sock_send[k]);
			}
		}
	}
}

void creating_sockets_for_receiving(void *arg) {
	vector<Arg>* vals = (vector<Arg> *)arg;
	int size = vals->size();
	for(int i = 0; i < size; i++) {
		(*vals)[i].sock_recv = for_receiving(PORT+i);
	}
}

void create_sockets_for_sending(void *arg) {
	ofstream fout("sockets.txt");
	vector<Arg>* vals = (vector<Arg> *)arg;
	int size = vals->size();
	for(int i = 0; i < size; i++){
		multimap<string,int>::iterator it;
		string out = (*vals)[i].data_type_output[0];
		if(out != "") {
			it = data_map.find(out);
			for(int k = 0; k < data_map.count(out); k++) {
				int port = it -> second;
				(*vals)[i].sock_send[k] = create_socket(port);
				fout << (*vals)[i].data_type_output[0] << " " << it -> second << " " << (*vals)[i].sock_send[k] << endl;
			}
		}
	}
	fout.close();
}

uint64_t timestamp() {
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (static_cast<uint64_t>(ts.tv_sec) * 10000000u
			+ static_cast<uint64_t>(ts.tv_nsec) / 100u) * 100u;
}

int main() {

	vector<pthread_t> thids(SIZE);
	vector<Arg> vals = parser();
	int size = vals.size();
	cout << size << endl;
    /**/
	for (int i = 0; i < size; i++) {
			if (pthread_create(&thids[i], (pthread_attr_t *) NULL, module,
					&vals[i])) {
				cerr << "Error on thread create!\n";
				exit(EXIT_FAILURE);
			}
	}

	void *arg = &vals;
	if (pthread_create(&thids[size], (pthread_attr_t *) NULL, thread_sync,
			arg)) {
		cerr << "Error on thread create!\n";
		exit(EXIT_FAILURE);
	}
	cout << "join\n";
	for (vector<pthread_t>::iterator it = thids.begin(); it != thids.end();
			++it) {
		pthread_join(*it, (void **) NULL);
	}
	cout << "afterjoin\n";

	write_result_into_file(vals, size);
    //*/
	return 0;
}

void *module(void *arg) {
	Arg *arguments = (Arg *) arg;
	long long int result = 1;
	bool flag1 = true;
	int count = 0;
	int sock, sock2 = for_receiving(arguments->port1);

    bool ifsend1 = false;
	double counter = 0.5;
	double current = arguments->data_amount;
	cout << arguments->addition_name << endl;
	sock = accept(sock2, NULL, NULL);
	if (sock < 0) {
		perror("accept");
		cerr << arguments->port1 << endl;
		close_all_sockets(arg);
		exit(EXIT_FAILURE);
	}
	int sock1 = -1;
	if (arguments->data_type_input[0] != "СС") {
		sock1 = accept(sock2, NULL, NULL);
		if (sock1 < 0) {
			perror("accept");
			cerr << arguments->port1 << endl;
			close_all_sockets(arg);
			exit(EXIT_FAILURE);
		}
//		int flags = fcntl(sock1,F_GETFL,0);
//	//	    assert(flags != -1);
//		if (flags == -1) {
//			perror("fcntl");
//			exit(EXIT_FAILURE);
//		}
//		flags = fcntl(sock1, F_GETFL, 0);
//		if ((flags & O_NONBLOCK) == O_NONBLOCK) {
//			printf("it's nonblocking");
//		}
//		else {
//			printf("it's blocking");
//		}
	}
	int sends_count = 0;
	while (flag1) {
		/**if (sock1 > 0 && sends_count >= 8) {
			if (recv(sock1, &result, sizeof(long long int), MSG_DONTWAIT) < 0) {
				puts("tred_sync recv error");
				perror("recv");
				exit(EXIT_FAILURE);
			}
			flag1 = false;
			close(sock);
			cout << arguments->port1 << endl;
			break;
		} //*/
		//cout << arguments->port1 << sock << endl;
		if (arguments->data_type_input[0] != "СС") {
			if (recv(sock1, &result, sizeof(long long int), MSG_DONTWAIT) <  0) {
//				puts("tred_sync recv error");
//				puts(arguments->addition_name);
//				perror("recv");
				//exit(EXIT_FAILURE);
			}
			else if (result == 0) {
				puts("sock1 result is 0");
				flag1 = false;
				close(sock1);
				close(sock);
				return NULL;
			}
		}
		//puts(arguments->addition_name);
		if (recv(sock, &result, sizeof(long long int), MSG_DONTWAIT) <  0) {
//			puts("second recv error");
//			puts(arguments->addition_name);
//			perror("recv");
			//exit(EXIT_FAILURE);
		}
		else if (result == 0) {
			puts("sock result is 0");
			flag1 = false;
			close(sock1);
			close(sock);
			return NULL;
		}
		else {
			sends_count++;

			output[arguments->number][count++] = 2;
			output[arguments->number][count++] = (long long int)timestamp();
//			close(sock);
			for (long long int i = 0; i < 10; i++) {
				result = 1;
				for (int k = 1; k <= 100; k++) {
					result = result * k;
				}
			}
			string out = arguments->data_type_output[0];
            //если выходные данные есть
            if(out != "") {
                //счетчики для принятия решения об отправлении данных
                if(current < counter) {
		            ifsend1 = false;
	            }
	            else {
		            ifsend1 = true;
		            counter += 1.0;
	            }
	            current += arguments->data_amount;
                if(ifsend1) {
				    for(int i = 0; i < data_map.count(out); i++) {
                        //мы их формируем
                        for(int m = 0; m < arguments->time_form[0]; m++) {
					        result = 1;
					        for(int k = 1; k <= 10; k++){
						        result = result * k;
					        }
				        }
                        //и вот теперь отправляем
            			for(int k = 0; k < data_map.count(out); k++) {
            				cout << "in sending loop\n";
							output[arguments->number][count++] = 1;
							output[arguments->number][count++] = (long long int)timestamp();
							output[arguments->number][count++] = arguments->sock_send[k];
							cout << arguments->sock_send[k] << endl;
							if (send(arguments->sock_send[k], &result, sizeof(long long int), 0) < 0) {
								puts("send error");
								cerr << arguments->addition_name << endl;
								perror("sending");
								exit(EXIT_FAILURE);
							}
							cout << arguments->addition_name << "send\n";
            			}
            			//puts("in more external loop\n");
				    }
				   //puts("in ifsend1\n");
                }
                //puts("in if out\n");
			}
            //puts("in else\n");
		}

//		puts("all right in recv");
//		cout << result << arguments->addition_name << "recv\n";
//		if (result == 0) {
//			flag1 = false;
//			close(sock);
//		}
		//puts("in while\n");
	}
	//puts("after while");
	close(sock2);
	return NULL;
}

void *thread_sync(void *arg) {
	vector<Arg>* vals = (vector<Arg> *)arg;
	int count = 0;
	int size = vals->size();
	int l = 0;
	int sockets[size];
	cout << (*vals)[0].addition_name << endl << (*vals)[1].addition_name << endl;
	cout << (*vals)[0].data_type_input[0] << endl << (*vals)[1].data_type_input[0] << endl;

	for(int i = 0; i < size; i++) {

		if((*vals)[i].data_type_input[0] == "СС")
			l++;
	}
	//cout << l << endl;
	cout << "ready to create sockets\n";
	create_sockets_for_sending(arg);
	cout << (*vals)[0].sock_send[0] << endl;
	long long int result = 60000;
	for(int i = 0; i < size; i++) {
		sockets[i] = create_socket(PORT+i);
	}
	for(int k = 0; k < NUMBER; k++){
		for (int i = 0; i < l; i++) {
			if (send(sockets[0], &result, sizeof(long long int), 0) < 0) {
				perror("send");
				exit(EXIT_FAILURE);
			}
			count = writing(size, count, i);
		}
		sleep(1);
	}
	output[size+1][count] = 5;
	count++;
	sleep(TIME);
	result = 0;
	cout << "thread_sync before ending\n";

	for (int i = 0; i < size; i++) {
		if (send(sockets[i], &result, sizeof(long long int), 0) < 0) {
			perror("send2");
			exit(EXIT_FAILURE);
		}
		count = writing(size, count, i);
	}
	for (int i = 0; i < size; i++) close(sockets[i]);
	close_all_sockets(arg);
	return NULL;
}

int writing(int size, int count, int i) {
	output[size+1][count] = (long long int)timestamp();
	count++;
	output[size+1][count] = PORT + i;
	count++;
	return count;
}

int for_receiving(int port) {
	int sock;
	struct sockaddr_in addr2;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	addr2.sin_family = AF_INET;
	addr2.sin_port = htons(port);
	addr2.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (bind(sock, (struct sockaddr *) &addr2, sizeof(addr2)) < 0) {
		cerr << port << endl;
		perror("bind");
		exit(EXIT_FAILURE);
	}
	listen(sock, 50);
	return sock;
}

vector<Arg> parser() {

	ifstream fin("modules.txt");
	ofstream fout("config.txt");
	ofstream fo("check.txt");
	char buff[SIZE];
	int i = 0, l = 0;
	int k = 0, m = 0;

	vector<Arg> vals;
	char tmp[SIZE];
	int number_of_flows = 0;

	while (fin >> tmp) {
		if (strcmp(tmp, "-") == 0) {
			i = i - 1;
			k = k + 1;
		} else {
			if(number_of_flows > 1) {
				i--;
				cout << vals[i].name << endl;
				number_of_flows--;
				cout << number_of_flows << endl;
				for(int m = 0; m < number_of_flows; m++) {
					vals.push_back(vals[i]);
				}
				string s = to_string(1);
				char const* pchar = s.c_str();
				strcat(vals[i].addition_name, pchar);
				for(int m = 1; m < number_of_flows+1; m++) {
					s = to_string(m+1);
					pchar = s.c_str();
					strcat(vals[i+m].addition_name, pchar);
					vals[i+m].port1 = vals[i].port1 + m;
					cout << k << endl;
					for(int z = 0; z < k + 1; z++)
					    data_map.insert(pair<string, int>(vals[i+m].data_type_input[z], vals[i+m].port1));
				}
				i = i + number_of_flows + 1;
			}
			fin >> number_of_flows;
			fo << endl;
			Arg arg;
			strcpy(arg.name, tmp);
			strcpy(arg.addition_name, tmp);
			vals.push_back(arg);
			k = 0;
			fo << vals[i].name << " ";
			vals[i].port1 = PORT + i;
			fo << vals[i].port1 << "	data_amount	";
			fin >> vals[i].data_amount;
			fo << vals[i].data_amount << "      ";
		}
		fin >> buff;
		if (strcmp(buff, "-") != 0) {
			vals[i].data_type_input[k] = buff;
			fo << "      data_input      " << vals[i].data_type_input[k]
					<< " k = " << k << "		";
			fout << buff;
			data_map.insert(pair<string, int>(buff, vals[i].port1));
			fout << " ";
			fin >> vals[i].time_hand[k];

			fout << vals[i].port1 << "\n";
			fo << vals[i].time_hand[k] << "	";
		} else
			fin >> buff;
		fin >> buff;
		if (strcmp(buff, "-") != 0) {
			vals[i].data_type_output[k] = buff;
			fin >> vals[i].time_form[k];
			fo << "		data_output " << vals[i].data_type_output[k]
					<< vals[i].time_form[k] << " k = " << k << endl;
		} else
			fin >> buff;
		i++;
	}
	fin.close();
	fout << endl;
	ofstream fout_map("map.txt");
	fo.close();
	for(int i = 0; i < vals.size(); i++) {
	    vals[i].number = i;
		fout << vals[i].addition_name << "  " << vals[i].port1 << endl;
	}
	for (multimap<string,int>::iterator it = data_map.begin(); it != data_map.end(); ++it)
	{
		fout_map << it->first << " " << it->second << endl;
	}
	fout.close();
	fout_map.close();
	return vals;
}

int create_socket(int port) {
	int sock1;
	struct sockaddr_in addr1;
	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock1 < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	addr1.sin_family = AF_INET;
	addr1.sin_port = htons(port);
	addr1.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (connect(sock1, (struct sockaddr *) &addr1, sizeof(addr1)) < 0) {
		perror("connect");
		cerr << port << endl;
		exit(EXIT_FAILURE);
	}
    //cout << "cl" << port << endl;
    return sock1;
}

void write_result_into_file(vector<Arg> vals, int size) {
    ofstream fout("output.txt");
    int l = 0;

    while(output[size+1][l] != 0) {
    	if(output[size+1][l] == 5) {
    		fout << "Цикл завершен" << endl;
    		l++;
    	}
    	fout << "SyncS в ";
    	fout << output[size+1][l] << " " ;
    	l++;
    	fout << "отправил на " << output[size+1][l] << " " ;
    	fout << endl;
    	l++;
    }
    fout << endl;

    long long int time = 0;
    for(int i = 0; i < size; i++) {
    	cout << i << " " << vals[i].addition_name << endl;
        	l = 0;
        	cout << output[i][l] << endl;
        	while((output[i][l] != 0)&(l < 200)){

                if(output[i][l] == 2) {
                	l++;
                	time = output[i][l];
                	l++;
               	}
                if(output[i][l] == 1) {
                	l++;
                	fout << vals[i].addition_name << " " << vals[i].port1 << " ";
                	fout << "принял в " << time;
                    fout << " в " << output[i][l] << " ";
                    l++;
                	fout << "отправил на " << output[i][l] << " ";
                	l++;
                	fout << endl;
              	}
                else {
                	fout << vals[i].addition_name << " " << vals[i].port1 << " ";
                	fout << "принял в " << time;
                	fout << endl;
                }
                if((output[i][l] != 2)&&(output[i][l] != 1)) {
                	//cout << output[i][l] << endl;
                	l++;
                }
        	}
            fout << endl;
        }
}
