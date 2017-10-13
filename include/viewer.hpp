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

Viewer::Viewer(const Viewer& v):  argc(v.argc), argv(v.argv), state(v.state) {}

Viewer& Viewer::operator=(const Viewer& v)
{
    argc = v.argc;
    argv = v.argv;
    state = v.state;
    return *this;
}

Viewer::Viewer(int c, char** v): argc(c), argv(v), state("init") {}

void Viewer::error(const string& s)
{
    cerr << "Error: " << s << endl;
    cerr << "For help type: " << endl << argv[0] << " --help" << endl;   
}

int Viewer::upd_state(const string& s)
{
    state = s; 
    return 0;
}

void Viewer::get_status() const
{

}