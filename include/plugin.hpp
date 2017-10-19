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
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <errno.h>

#include "io.h"
#include "matrix.h"
#include "mvc.h"

using std::string;

#define MAX_NUM_LIBS 10

class Plugin
{
    string name;
public:
    Plugin();
    
    string get_name();
    //do filter in library .so
    virtual Image processing(Image, const int r, double fr);
    virtual ~Plugin(){}
    
};

