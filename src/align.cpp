#include <assert.h>
#include <string>

#include "mvc.h"
#include "shiftCalc.hpp"

using std::string;
using std::cout;
using std::cerr;
using std::endl;


Image Modeler::align(Image srcImage, bool isPostprocessing, std::string postprocessingType, double fraction, bool isMirror, 
            bool isInterp, bool isSubpixel, double subScale)
{
    //split pic
    auto resImage = srcImage;
    optShift GR_align;
    optShift GB_align;
    if ( (srcImage.n_rows > 900) && (srcImage.n_cols > 900) ) {
        cerr << "pyramida" << endl; 
        set_state("starts pyramid scaling");
        send_update();       
        Pyramida pyr(srcImage,GR_align,GB_align);
        pyr = calc_pyramid(pyr);
        set_state("pyramid scaling complete");
        send_update();  
        resImage =  pyr.res;
        GR_align = pyr.GR;
        GB_align = pyr.GB;
    } else {
    
        optShift opt;
        auto part = srcImage.n_rows / 3;
        Image b = srcImage.submatrix(0, 0, part, srcImage.n_cols);
        Image g = srcImage.submatrix(part, 0, part, srcImage.n_cols);
        Image r = srcImage.submatrix(2 * part, 0, part, srcImage.n_cols); 
        set_state("image splitted up into color channels");
        send_update();
        //optimal shift calc
        set_state("starts channels alignment");
        const ssize_t limit = 18;
        GR_align = optimalAlign(g, r, opt, limit);
        GB_align = optimalAlign(g, b, opt, limit);
        //imposition
        resImage = imposition(GR_align,GB_align,r,g,b);
        set_state("channels alignment complete");
        send_update();
    }
    //cut borders
    resImage = imposition_cut(resImage,GR_align,GB_align);
    //postprocessing
    if (isPostprocessing){
        set_state("starts postprocessing");
        send_update();
        if (postprocessingType == "--gray-world")
            resImage = gray_world(resImage);
        if (postprocessingType == "--autocontrast")
            resImage = autocontrast(resImage, fraction);
        //radius-frame processing from here
        int radius_filter = 1; //frame = (2*filrer_radius+1)*(2*filrer_radius+1) 
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


Image Modeler::sobel_x(Image src_image) {
    Matrix<double> kernel = {{-1, 0, 1},
                             {-2, 0, 2},
                             {-1, 0, 1}};
    return custom(src_image, kernel);
}

Image Modeler::sobel_y(Image src_image) {
    Matrix<double> kernel = {{ 1,  2,  1},
                             { 0,  0,  0},
                             {-1, -2, -1}};
    return custom(src_image, kernel);
}

Image Modeler::unsharp(Image src_image) {
    // cerr << "unsharp" << endl;
    set_state("postprocessing::starts unsharping");
    send_update();
    
    Matrix<double> kernel = {{-1.0/6.0, -2.0/3.0, -1.0/6.0},
                             {-2.0/3.0, 13.0/3.0, -2.0/3.0},
                             {-1.0/6.0, -2.0/3.0, -1.0/6.0}};
    Image tmp = custom(src_image,kernel);
    set_state("postprocessing::unsharping complete");
    send_update();
    return tmp;
}

Image Modeler::gray_world(Image src_image) {
    // cerr << "gray-world" << endl;
    set_state("postprocessing::starts gray-world");
    send_update();
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
        
    set_state("postprocessing::gray-world complete");
    send_update();
    
    return src_image;
}

Image Modeler::resize(Image src_image, double scale) {
    return calc_scale(src_image,scale);
}

Image Modeler::custom(Image src_image, Matrix<double> kernel) {
    return src_image.unary_map(unaryOp(kernel));  
}

Image Modeler::autocontrast(Image src_image, double fraction) {
    // cerr << "autocontrast" << endl;
    set_state("postprocessing::starts autocontrast");
    send_update();
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
    set_state("postprocessing::autocontrast complete");
    send_update();
    return src_image;
}

Image Modeler::gaussian(Image src_image, double sigma, int radius)  {
    return src_image;
}

Image Modeler::gaussian_separable(Image src_image, double sigma, int radius) {
    return src_image;
}

Image Modeler::median(Image src_image, int radius) {
    // cerr << "median " << radius << endl;
    set_state("postprocessing::starts median");
    send_update();
    src_image = mirror(src_image, radius);
    src_image = src_image.unary_map(Median(src_image,radius));
    set_state("postprocessing::median complete");
    send_update();
    return mirror_crop(src_image, radius); 
    
}


Image Modeler::median_linear(Image src_image, int radius) {
    return src_image;
}

Image Modeler::median_const(Image src_image, int radius) {
    return src_image;
}

Image Modeler::canny(Image src_image, int threshold1, int threshold2) {
    return src_image;
}
