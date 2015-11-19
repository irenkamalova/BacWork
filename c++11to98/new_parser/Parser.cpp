#include <iostream>
#include "Module.h"
#include <cstring>
#include <fstream>
#include <map>
#include <stdlib.h>  //atoi


using namespace std;

const int PORT = 60000;
string s1 = "/home/newuser/ClionProjects/BacWork/c++11to98/new_parser/file1.txt";
string s2 = "/home/newuser/ClionProjects/BacWork/c++11to98/new_parser/file2.txt";
string s3 = "/home/newuser/ClionProjects/BacWork/c++11to98/new_parser/file3.txt";
const short NUMBER_OF_QUEUE_CH = 50;
const int LENGTH_OF_ARRAY = 300;
int datas[NUMBER_OF_QUEUE_CH][LENGTH_OF_ARRAY];
vector<pair<int*, int*> > pairs;

vector<Module> parser1();
map<string, int> parser2();
map<int, string> parser3();

int main(int argc, char *argv[]) {
    int my_machine = atoi(argv[1]);
    vector<Module> modules = parser1();
    map<string, int> module_machine = parser2();
    map<int, string> machine_address = parser3();
    int pairs_counter;
    int port = PORT;


    for(int i = 0; i < modules.size(); i++) {
        modules[i].set_machine(module_machine[modules[i].get_name()]);
        modules[i].set_my_ip_address(machine_address[modules[i].get_machine()]);
    }
    string check1, check2, check3, check4;
    for(int i = 0; i < modules.size(); i++) {
        for(int i_m = 0; i_m < modules[i].get_nto(); i_m++) {
            modules[i].message_output_array[i_m];
            for(int j = i + 1; j < modules.size(); j++) {
                for(int j_m = 0; j_m < modules[j].get_nti(); j_m++) {
                    check1 = modules[i].message_output_array[i_m].name;
                    check2 =  modules[j].message_input_array[j_m].name;
                    check3 = modules[i].message_output_array[i_m].name_to;
                    check4 = modules[j].get_name();
                    if( (modules[i].message_output_array[i_m].name == modules[j].message_input_array[j_m].name)
                            && (modules[i].message_output_array[i_m].name_to == modules[j].get_name()) ) {
                        if(modules[i].get_machine() == modules[j].get_machine()) {
                            modules[i].message_output_array[i_m].connection_type = 0;
                            modules[j].message_input_array[j_m].connection_type = 0;
                            pairs.push_back(make_pair(&datas[pairs.size() - 1][0], &datas[pairs.size() - 1][0]));
                            modules[i].message_output_array[i_m].channel_to = pairs.size() - 1;
                            modules[j].message_input_array[j_m].channel_from = pairs.size() - 1;
                        }
                        else {
                            modules[i].message_output_array[i_m].connection_type = 1;
                            modules[j].message_input_array[j_m].connection_type = 1;
                            modules[j].set_port(port++);
                            modules[i].message_output_array[i_m].port_to = modules[j].get_port();
                            modules[i].message_output_array[i_m].ip_address_to = modules[j].get_my_ip_address();
                            modules[j].message_input_array[j_m].ip_address_from = modules[i].get_my_ip_address();
                        }
                    }
                }
            }
        }
    }


    vector<Module> my_modules;
    for(int i = 0; i < modules.size(); i++) {
        if(modules[i].get_machine() == my_machine) {
            my_modules.push_back(modules[i]);
        }
    }
    cout << "finish" << endl;
    return 0;
}



map<int, string> parser3() {
    map<int, string> machine_address;
    char * cstr = new char [s3.length()+1];
    strcpy(cstr, s3.c_str());
    ifstream fin(cstr);
    char buff[SIZE];
    string address;
    int machine = 0;
    while (fin >> buff) {
        fin >> buff;
        machine = atoi(buff);
        fin >> buff;
        address = buff;
        machine_address[machine] = address;
    }
    return machine_address;

};

map<string, int> parser2() {
    map<string, int> module_machine;
    char * cstr = new char [s2.length()+1];
    strcpy(cstr, s2.c_str());
    ifstream fin(cstr);
    char buff[SIZE];
    string module;
    int machine;
    while (fin >> buff) {
        module = buff;
        fin >> machine;
        module_machine[module] = machine;
    }
    return module_machine;
}


vector<Module> parser1() {
    char * cstr = new char [s1.length()+1];
    strcpy(cstr, s1.c_str());
    ifstream fin(cstr);
    char buff[SIZE];
    int i = 0;//number of modules
    int number_of_mes_input;
    int number_of_mes_output;

    vector<Module> vals;
    while (fin >> buff) {
        if (strcmp(buff, "-") == 0) {
            i--;
        } else {
            Module *module = new Module;
            module->set_name(buff);
            vals.push_back(*module);
            vals[i].set_number(i);
            number_of_mes_input = 0; // number of input data
            vals[i].set_nti(number_of_mes_input);
            number_of_mes_output = 0; // number of output data
            vals[i].set_nto(number_of_mes_output);

            vals[i].set_port(0);
            //bool aff;
            //fin >> aff;
            //vals[i].set_affectation(aff);

            double amount;
            fin >> amount;
            vals[i].set_data_amount(amount);
        }
        fin >> buff;
        if (strcmp(buff, "-") != 0) {

            Module::message_input m_i;

            m_i.name = buff;

            int time_hand;
            fin >> time_hand;
            m_i.time_hand = time_hand;

            bool parameter;
            fin >> parameter;
            m_i.parameter = parameter;

            vals[i].set_message_input(m_i, vals[i].get_nti());
            vals[i].inc_nti();
        }
        fin >> buff;
        if (strcmp(buff, "-") != 0) {

            Module::message_output m_o;

            m_o.name = buff;

            int time_form;
            fin >> time_form;
            m_o.time_form = time_form;



            fin >> buff;
            m_o.name_to = buff;

            vals[i].set_message_output(m_o, vals[i].get_nto());
            vals[i].inc_nto();
        }
        i++;
    }

    fin.close();
    return vals;
}