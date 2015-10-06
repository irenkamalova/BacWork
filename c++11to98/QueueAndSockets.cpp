//
// Created by Irina on 05.10.15.
//

#include "QueueAndSockets.h"
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <time.h> 	 //clock_gettime()
#include <unistd.h> //sleep
#include <stdlib.h> //exit
#include <cstring>
///// Library for sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

using namespace std;

QueueAndSockets::QueueAndSockets() { }

QueueAndSockets::~QueueAndSockets() { }

void QueueAndSockets::run(vector<Module> m) {
    for(vector<Module>::iterator it = m.begin(); it != m.end();
        ++it) {
        cout << it->get_name() << endl;
    }
}

