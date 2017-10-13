#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <unistd.h>
#include <ctime>
#include <climits>

#include "io.h"
#include "matrix.h"

// #include "align.h"

using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::numeric_limits;

class Viewer
{
    int argc;
    char **argv;    
    string state;
public:
    Viewer(int c, char** v);
    Viewer(const Viewer& v);
    Viewer& operator=(const Viewer& v);
    
    void error(const string& s); //catch(error) 
    void get_status() const; 
    int upd_state(const string& s);
};    


class Modeler
{

public:


    //alignment
    Image align(Image srcImage, bool isPostprocessing, std::string postprocessingType, double fraction, bool isMirror, 
        bool isInterp, bool isSubpixel, double subScale);  
    //filters
    Image sobel_x(Image src_image);
    Image sobel_y(Image src_image);
    Image unsharp(Image src_image);
    Image gray_world(Image src_image);
    Image resize(Image src_image, double scale);
    Image custom(Image src_image, Matrix<double> kernel);
    Image autocontrast(Image src_image, double fraction);
    Image gaussian(Image src_image, double sigma, int radius);
    Image gaussian_separable(Image src_image, double sigma, int radius);
    Image median(Image src_image, int radius);
    Image median_linear(Image src_image, int radius);
    Image median_const(Image src_image, int radius);
    Image canny(Image src_image, int threshold1, int threshold2);

};

class Controler
{
    Modeler model;

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

