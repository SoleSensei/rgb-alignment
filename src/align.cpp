#include "align.h"
#include <assert.h>
#include <string>

#include "shiftCalc.hpp"

using std::string;
using std::cout;
using std::cerr;
using std::endl;


Image align(Image srcImage, bool isPostprocessing, std::string postprocessingType, double fraction, bool isMirror, 
            bool isInterp, bool isSubpixel, double subScale)
{
    //split pic
    auto part = srcImage.n_rows / 3;
    Image b = srcImage.submatrix(0, 0, part, srcImage.n_cols);
    Image g = srcImage.submatrix(part, 0, part, srcImage.n_cols);
    Image r = srcImage.submatrix(2 * part, 0, part, srcImage.n_cols); 

    //optimal shift calc
    const ssize_t limit = 18;
    auto GR_align = optimalAlign(g, r, limit );
    auto GB_align = optimalAlign(g, b, limit );
    //imposition
    for (ssize_t i = 0; i < r.n_rows; i++) {
        for (ssize_t j = 0; j < r.n_cols; j++){ 
            ssize_t v1,h1,v2,h2;
            std::tie(v1,h1,v2,h2) = setBound(i,j,GR_align,GB_align,g);
            g(i, j) = std::make_tuple(std::get<0>(r(v1, h1)), 
                                      std::get<0>(g(i, j)),
                                      std::get<0>(b(v2, h2)));
        }
    }
    
    //cut borders
    auto resImage = imposition_cut(g,GR_align,GB_align);
    
    //postprocessing
    if (isPostprocessing){
        if (postprocessingType == "--gray-world")
            resImage = gray_world(resImage);
        if (postprocessingType == "--autocontrast")
            resImage = autocontrast(resImage, fraction);
        //radius-frame processing from here
        int radius_filter = 50; //frame = (2*filrer_radius+1)*(2*filrer_radius+1) 
        if (isMirror)
            resImage = mirror(resImage, radius_filter);
        if (postprocessingType == "--unsharp")
                resImage = unsharp(resImage);
        //crop mirrored parts
        if (isMirror) 
            resImage = mirror_crop(resImage, radius_filter); 
    }
    return resImage;
}


Image sobel_x(Image src_image) {
    Matrix<double> kernel = {{-1, 0, 1},
                             {-2, 0, 2},
                             {-1, 0, 1}};
    return custom(src_image, kernel);
}

Image sobel_y(Image src_image) {
    Matrix<double> kernel = {{ 1,  2,  1},
                             { 0,  0,  0},
                             {-1, -2, -1}};
    return custom(src_image, kernel);
}

Image unsharp(Image src_image) {
    std::cerr << "unsharp" << std::endl;
    
    Matrix<double> kernel = {{-1.0/6.0, -2.0/3.0, -1.0/6.0},
                             {-2.0/3.0, 13.0/3.0, -2.0/3.0},
                             {-1.0/6.0, -2.0/3.0, -1.0/6.0}};
    return custom(src_image,kernel);
}

Image gray_world(Image src_image) {
    std::cerr << "gray-world" << std::endl;
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

Image resize(Image src_image, double scale) {

    

    return src_image;
}

Image custom(Image src_image, Matrix<double> kernel) {
    
    return src_image.unary_map(unaryOp(kernel));  
}

Image autocontrast(Image src_image, double fraction) {
    std::cerr << "autocontrst" << std::endl;
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

Image gaussian(Image src_image, double sigma, int radius)  {
    return src_image;
}

Image gaussian_separable(Image src_image, double sigma, int radius) {
    return src_image;
}

Image median(Image src_image, int radius) {
    std::cerr << "median " << radius << std::endl;
    
    return src_image.unary_map(Median(src_image,radius));
}


Image median_linear(Image src_image, int radius) {
    return src_image;
}

Image median_const(Image src_image, int radius) {
    return src_image;
}

Image canny(Image src_image, int threshold1, int threshold2) {
    return src_image;
}