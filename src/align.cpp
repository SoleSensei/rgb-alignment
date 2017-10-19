#include <assert.h>
#include <string>

#include "mvc.h"
#include "filters.hpp"

using std::string;
using std::cerr;
using std::endl;

Modeler::Modeler(): state("Start initialization"), buf(), lb(this) 
{
    buf.clear();
}

void Modeler::set_state(const string& s){
    state = s;
}
string Modeler::get_state() const{
    return state;
}

//io images in io.cpp
Image Modeler::load(const char* src){
    set_state("Image loading");
    Image tmp = load_image(src);
    set_state("Image loaded");
    return tmp;
}
void Modeler::save(const Image& dst, const char* path){
    set_state("Image saving");
    save_image(dst, path);
    set_state("Image saved");
}

//============alignment============== 

Image Modeler::align(Image srcImage, bool isPostprocessing, std::string postprocessingType, double fraction, bool isMirror, 
            bool isInterp, bool isSubpixel, double subScale)
{
    //split pic
    auto resImage = srcImage;
    optShift GR_align;
    optShift GB_align;
    if ( (srcImage.n_rows > 900) && (srcImage.n_cols > 900) ) {
        set_state("Starts pyramid scaling");
        send_update();       
        Pyramida pyr(srcImage,GR_align,GB_align);
        pyr = calc_pyramid(pyr);
        set_state("Pyramid scaling complete");
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
        set_state("Image splitted up into color channels");
        send_update();
        //optimal shift calc
        set_state("Starts channels alignment");
        send_update();        
        const ssize_t limit = 18;
        GR_align = optimalAlign(g, r, opt, limit);
        GB_align = optimalAlign(g, b, opt, limit);
        //imposition
        resImage = imposition(GR_align,GB_align,r,g,b);
        set_state("Channels alignment complete");
        send_update();
    }
    //cut borders
    resImage = imposition_cut(resImage,GR_align,GB_align);
    //postprocessing
    if (isPostprocessing){
        set_state("Starts postprocessing");
        send_update();
        if (postprocessingType == "--gray-world")
            resImage = gray_world(resImage);
        if (postprocessingType == "--autocontrast")
            resImage = autocontrast(resImage, fraction);
        //radius-frame processing from here
        int radius_filter = 1; //frame = (2*filter_radius+1)*(2*filter_radius+1) 
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
 
int Modeler::search_filters()
{
    set_state("searching plugins...");
    send_update();
    int loaded = lb.load_libs();
    lb.print_loaded();
    return loaded;
}

Image Modeler::do_filter(Image src_image, int num)
{ 
    string filter_name = lb.choosen_filter(num);
    buf += "\"" + filter_name + "\" is applying\n";
    set_state(filter_name);
    send_update();
    send_update();
    if(filter_name == "align") return src_image;
    Image dst = lb.do_plugin(src_image, filter_name);
    return dst;
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
    set_state("Postprocessing::unsharping");
    send_update();
    buf += "searching plugins... \n";
    lb.load_libs();
    lb.print_loaded();
    string filter = "unsharp";
    Image dst = lb.do_plugin(src_image, filter);
    send_update();    
    return dst;
}

Image Modeler::gray_world(Image src_image) {
    set_state("Postprocessing::gray-world");
    send_update();
    buf += "searching plugins... \n";
    lb.load_libs();
    lb.print_loaded();
    string filter = "gray-world";
    Image dst = lb.do_plugin(src_image, filter);
    send_update();    
    return dst;
}

Image Modeler::resize(Image src_image, double scale) {
    return calc_scale(src_image,scale);
}

Image Modeler::custom(Image src_image, Matrix<double> kernel) {
    return src_image.unary_map(unaryOp(kernel));  
}

Image Modeler::autocontrast(Image src_image, double fraction) {
    set_state("Postprocessing::autocontrast");
    send_update();
    buf += "searching plugins... \n";
    lb.load_libs();
    lb.print_loaded();
    string filter = "autocontrast";
    Image dst = lb.do_plugin(src_image, filter);
    send_update();    
    return dst;
}

Image Modeler::gaussian(Image src_image, double sigma, int radius)  {
    set_state("Postprocessing::gaussian");
    send_update();
    buf += "searching plugins... \n";
    lb.load_libs();
    lb.print_loaded();
    string filter = "gaussian";
    Image dst = lb.do_plugin(src_image, filter, radius);
    send_update();    
    return dst;
}

Image Modeler::gaussian_separable(Image src_image, double sigma, int radius) {
    set_state("Postprocessing::gaussian_separable");
    send_update();
    buf += "searching plugins... \n";
    lb.load_libs();
    lb.print_loaded();
    string filter = "gaussian-separable";
    Image dst = lb.do_plugin(src_image, filter, radius);
    send_update();    
    return dst;
}

Image Modeler::median(Image src_image, int radius) {
    set_state("Postprocessing::median");
    send_update();
    buf += "searching plugins... \n";
    lb.load_libs();
    lb.print_loaded();
    string filter = "median";
    Image dst = lb.do_plugin(src_image, filter, radius);
    send_update();    
    return dst;
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
