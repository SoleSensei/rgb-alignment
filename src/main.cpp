#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <unistd.h>
#include <ctime>

#include "ConsoleControler.hpp"
#include "ConsoleViewer.hpp"

int main(int argc, char **argv)
{
    Modeler img;
    ConsoleViewer view(argc, argv, &img); 
    ConsoleControler cc(&img,&view);
    auto return_value = cc.align_calc(argc, argv);

    return return_value;
}
