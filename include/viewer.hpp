#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <unistd.h>
#include <ctime>

#include "mvc.h"

using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::numeric_limits;




//-----
ConsoleViewer::ConsoleViewer(int c, char** v, Modeler* m): argc(c), argv(v), model(m) 
{
    model->add_view(this);
}
ConsoleViewer::ConsoleViewer(const ConsoleViewer& v):  argc(v.argc), argv(v.argv), model(v.model) {}

ConsoleViewer& ConsoleViewer::operator=(const ConsoleViewer& v)
{
    argc = v.argc;
    argv = v.argv;
    return *this;
}


void ConsoleViewer::error(const string& s)
{
    cerr << "Error: " << s << endl;
    cerr << "For help type: " << endl << argv[0] << " --help" << endl;   
}

void ConsoleViewer::update()
{   
    cout << "Current state: ";
    cout << model->get_state() << endl;

}