/*
 * Socket.h
 *
 *  Created on: Apr 22, 2015
 *      Author: irisha
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <vector>
#include <map>
#include "Module.h"

class Socket {
public:
	Socket();
	virtual ~Socket();
    enum modules_types {AUTO, SEARCH} m_t;	
    int run(int flows_auto, int flows_search);
	void create_sockets(vector<Module>* vals, int num_object, string s);
	void set_general(vector<Module> arg);
	uint64_t timestamp();
	int create_socket(int port);
	void close_all_sockets(void *arg);
	void modules_sockets(vector<Module>* vals);
	void write_to_file(vector<Module>* vals, int num_object);
	void set_index_for_file(int index);
	void module_socket(Module *vals);
	void create_ports(vector<Module> vals, int num_object, modules_types m_t);
	int create_sock_for_receiving(int port);
    
    
private:
	multimap<string, int> data_map;
	const int PORT = 60100;
	int port;
	uint64_t starttime;
	vector<Module> general;
	long long int array_for_file[203][200000];
	vector< vector<Module> > auto_accomp;
	vector< vector<Module> > search;
	int index_for_file;
};


#endif /* SOCKET_H_ */
