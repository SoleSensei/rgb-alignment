#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <unistd.h>
#include <ctime>

#include "align.h"

using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::numeric_limits;

class Control
{

    void print_help(const char* argv0);
    
    template<typename ValueType>
    ValueType read_value(string s);

    template<typename ValueType>
    bool check_value(string s);
    
    template<typename ValueT>
    void check_number(string val_name, ValueT val, ValueT from, ValueT to);
    
    void check_argc(int argc, int from, int to);
    
    Matrix<double> parse_kernel(string kernel);
    
    void parse_args(char **argv, int argc, bool *isPostprocessing, string *postprocessingType, double *fraction, bool *isMirror, 
                bool *isInterp, bool *isSubpixel, double *subScale);

public:
    
    int align_calc(int argc, char** argv);
    
};

class Modeler
{



};

class Viewer
{

};