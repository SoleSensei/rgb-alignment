/*autocontast filter*/
#include <assert.h>
#include <string>

#include "../include/mvc.h"
#include "../include/filters.hpp"

#include "../include/plugin.hpp"

using std::string;
using std::cerr;
using std::endl;
using std::tuple;

extern "C" Plugin* create_object()
{
  return new Plugin;
}

extern "C" void destroy_object( Plugin* object )
{
  delete object;
}

Plugin::Plugin(): name("autocontrast") {}

Image Plugin::processing(Image src_image, const int radius, double fraction) {
    
    cerr << "autocontrast" << endl;
    double Ymin = 255;
    double Ymax = 0;
    double r_cl,g_cl,b_cl;
    for (uint i = 0; i < src_image.n_rows; ++i)
        for (uint j = 0; j < src_image.n_cols; ++j)
        {
            std::tie(r_cl,g_cl,b_cl)  = src_image(i,j);
            const double Y = 0.2125 * r_cl + 0.7154 * g_cl + 0.0721 * b_cl;
            Ymax = Y > Ymax ? Y : Ymax;
            Ymin = Y < Ymin ? Y : Ymin;            
        }
        Ymax = Ymax - (Ymax-Ymin)*fraction;
        Ymin = Ymin + (Ymax-Ymin)*fraction;
        assert(Ymax > Ymin);
    for (uint i = 0; i < src_image.n_rows; ++i)
        for (uint j = 0; j < src_image.n_cols; ++j)
        {
            std::tie(r_cl,g_cl,b_cl)  = src_image(i,j);
            double fr = (r_cl - Ymin) * 255 / (Ymax - Ymin);
            double fg = (g_cl - Ymin) * 255 / (Ymax - Ymin);
            double fb = (b_cl - Ymin) * 255 / (Ymax - Ymin);
        
            src_image(i,j) = overflow_crop(std::make_tuple(fr, fg, fb));
        }
    return src_image;
}