#include <iostream>
#include "Module.h"
#include <cstring>
#include <fstream>
#include <map>

using namespace std;

string s1 = "file1.txt";
string s2 = "file2.txt";
string s3 = "file3.txt";

vector<Module> parser1();
map<string, int> parser2();
map<int, string> parser3();

int main() {
    vector<Module> modules = parser1();
    map<string, int> module_machine = parser2();
    map<int, string> machine_address = parser3();
    int pairs_counter;


    for(int i = 0; i < modules.size(); i++) {
        modules[i].set_machine(module_machine[modules[i].get_name()]);
        modules[i].set_my_ip_address(machine_address[modules[i].get_machine()]);
        vector<Module::message_output>* m_o = &(modules[i].get_all_message_output());
        for (int k = 0; k < (*m_o).size(); k++) {
            if(modules[i].get_machine() == module_machine[(*m_o)[k].name_to]) {
                (*m_o)[k].connection_type = 0;
                (*m_o)[k].channel_to = pairs_counter;
                pairs_counter++;
            }
        }

    }

    return 0;
}



map<int, string> parser3() {
    map<int, string> machine_address;
    char * cstr = new char [s2.length()+1];
    strcpy(cstr, s2.c_str());
    ifstream fin(cstr);
    char buff[SIZE];
    string address;
    int machine;
    while (fin >> machine) {
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

            bool aff;
            fin >> aff;
            vals[i].set_affectation(aff);

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