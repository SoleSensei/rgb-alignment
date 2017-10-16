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

using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::numeric_limits;
using std::vector;
using std::ofstream;

//pure virtual function to viewers
class Observer
{
public:
    virtual void update() = 0;
    virtual ~Observer(){}
};

//add/update observers to model 
class Subject
{
    vector< class Observer* > views; //all viewers tracing model
public:
    Subject(): views(0) {}
    void add_view(Observer *obs)
    {
        views.push_back(obs);
    }
    void send_update()
    {
        for (uint i = 0; i < views.size(); i++)
            views[i]->update();
    }
};

//methods description in align.cpp
class Modeler: public Subject
{
    string state;
    void set_state(const string& s){
        state = s;
    }
public:
    Modeler(): state("Start initialization") {}
    string get_state() const{
        return state;
    }

    //io images in io.cpp
    Image load(const char* src){
        set_state("Image loading");
        Image tmp = load_image(src);
        set_state("Image loaded");
        return tmp;
    }
    void save(const Image& dst, const char* path){
        set_state("Image saving");
        save_image(dst, path);
        set_state("Image saved");
    }
    //alignment in align.cpp
    Image align(Image srcImage, bool isPostprocessing, std::string postprocessingType, double fraction, bool isMirror, 
        bool isInterp, bool isSubpixel, double subScale);  
    //filters in align.cpp and filters.cpp
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


//bond with model
class ConsoleViewer: public Observer
{
    int argc;
    char **argv;    
    Modeler* model;
    ofstream flog;    
public:
    ConsoleViewer(int c, char** v, Modeler* m);
    ConsoleViewer(const ConsoleViewer& v);
    ConsoleViewer& operator=(const ConsoleViewer& v);

    void update();
    
    void print_help(const char* argv0);
    void error(const string& s); //catch(error) 
};    

//bond with model and view
class ConsoleControler
{
    Modeler* model;
    ConsoleViewer* view;

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
    ConsoleControler(Modeler* m, ConsoleViewer* v);
    int align_calc(int argc, char** argv);
    
};    

