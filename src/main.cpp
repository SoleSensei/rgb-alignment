#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <unistd.h>
#include <ctime>

#include "control.hpp"
#include "viewer.hpp"

using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::numeric_limits;

int main(int argc, char **argv)
{
    Control c; //mvc controler
    Viewer v(argc, argv);  //mvc viewer
    try {
        c.align_calc(argc, argv);
    } catch (const string &s) {
        v.error(s);
        return 1;
    }
    catch (...)
    {
        v.error("unexpected");
        return 1;
    }
    return 0;
}
