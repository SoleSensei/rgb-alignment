/*gray-world filter*/
#include <assert.h>
#include <string>


#include "../../include/mvc.h"
#include "../../include/filters.hpp"

#include "../../include/plugin.hpp"

using std::string;
using std::cerr;
using std::endl;

extern "C" Plugin* create_object()
{
  return new Plugin;
}

extern "C" void destroy_object( Plugin* object )
{
  delete object;
}

Plugin::Plugin(): name("gray-world") {}

string Plugin::get_name()
{
    return name; 
}

extern "C" string get_name()
{
    Plugin p;
    return p.get_name();
}


Image Plugin::processing(Image src_image, const int radius, double fr){
    cerr << "gray-world" << endl;
    double r_cl = 0;
    double b_cl = 0;
    double g_cl = 0;
    double pic_size = src_image.n_cols * src_image.n_rows;
    //brightness calc
    for (uint i = 0; i < src_image.n_rows; ++i)
        for (uint j = 0; j < src_image.n_cols; ++j)
        {
            r_cl += double(std::get<0>(src_image(i,j))) / pic_size;
            g_cl += double(std::get<1>(src_image(i,j))) / pic_size;
            b_cl += double(std::get<2>(src_image(i,j))) / pic_size;

        }
    double sum = ( b_cl + g_cl + r_cl ) / 3;
    for (uint i = 0; i < src_image.n_rows; ++i)
        for (uint j = 0; j < src_image.n_cols; ++j)
            src_image(i,j) = overflow_crop(std::make_tuple(std::get<0>(src_image(i,j)) * (sum / r_cl),
                                                           std::get<1>(src_image(i,j)) * (sum / g_cl),
                                                           std::get<2>(src_image(i,j)) * (sum / b_cl)));
        
    return src_image;
}