#pragma once
#include <dlfcn.h>

#include "mvc.h"

ConsoleControler::ConsoleControler(Modeler* m, ConsoleViewer* v): model(m), view(v){}

int ConsoleControler::align_calc(int argc, char** argv)
{
 try{
    check_argc(argc, 2, numeric_limits<int>::max());
    if (string(argv[1]) == "--help") {
        view->print_help(argv[0]);
        return 0;
    }

    check_argc(argc, 4, numeric_limits<int>::max());
    Image src_image = model->load(argv[1]), dst_image;
    model->send_update();

    string action(argv[3]);

    if (action == "--sobel-x") {
        check_argc(argc, 4, 4);
        dst_image = model->sobel_x(src_image);
    } else if (action == "--sobel-y") {
        check_argc(argc, 4, 4);
        dst_image = model->sobel_y(src_image);
    } else if (action == "--unsharp") {
        check_argc(argc, 4, 4);
        dst_image = model->unsharp(src_image);
    } else if (action == "--gray-world") {
        check_argc(argc, 4, 4);
        dst_image = model->gray_world(src_image);
    } else if (action == "--resize") {
        check_argc(argc, 5, 5);
        double scale = read_value<double>(argv[4]);
        dst_image = model->resize(src_image, scale);
    }  else if (action == "--custom") {
        check_argc(argc, 5, 5);
        Matrix<double> kernel = parse_kernel(argv[4]);
        dst_image = model->custom(src_image, kernel);
    } else if (action == "--autocontrast") {
        check_argc(argc, 4, 5);
        double fraction = 0.0;
        if (argc == 5) {
            fraction = read_value<double>(argv[4]);
            check_number("fraction", fraction, 0.0, 0.4);
        }
        dst_image = model->autocontrast(src_image, fraction);
    } else if (action == "--gaussian" || action == "--gaussian-separable") {
        check_argc(argc, 5, 6);
        double sigma = read_value<double>(argv[4]);
        check_number("sigma", sigma, 0.1, 100.0);
        int radius = 3 * sigma;
        if (argc == 6) {
            radius = read_value<int>(argv[5]);
            check_number("radius", radius, 1, numeric_limits<int>::max());
        }
        if (action == "--gaussian") {
            dst_image = model->gaussian(src_image, sigma, radius);
        } else {
            dst_image = model->gaussian_separable(src_image, sigma, radius);
        }
    } else if (action == "--canny") {
        check_argc(6, 6, numeric_limits<int>::max());
        int threshold1 = read_value<int>(argv[4]);
        check_number("threshold1", threshold1, 0, 360);
        int threshold2 = read_value<int>(argv[5]);
        check_number("threshold2", threshold2, 0, 360);
        if (threshold1 >= threshold2)
            throw string("threshold1 must be less than threshold2");
        dst_image = model->canny(src_image, threshold1, threshold2);
    } else if (action == "--median" || action == "--median-linear" ||
                action == "--median-const") {
        check_argc(argc, 4, 5);
        int radius = 1;
        if (argc == 5) {
            radius = read_value<int>(argv[4]);
            check_number("radius", radius, 1, numeric_limits<int>::max());
        }
        if (action == "--median") {
            dst_image = model->median(src_image, radius);
        } else if (action == "--median-linear") {
            dst_image = model->median_linear(src_image, radius);
        } else {
            dst_image = model->median_const(src_image, radius);
        }
    } else if (action == "--align" || action == "--filter") {
        int loaded = 0;
        if (action == "--filter") {
            check_argc(argc,4,4);
            loaded = model->search_filters();
            if (loaded>0){
                model->buf+="Choose filter: ";
                model->send_update();
                int num;
                std::cin >> num;
                dst_image = model->do_filter(src_image, num);
            }
        }
        if(loaded == 0 || model->get_state() == "align"){
            bool isPostprocessing = false, isInterp = false,
            isSubpixel = false, isMirror = false;
            string postprocessingType;
            
            double fraction = 0.0, subScale = 2.0;
            
            if (argc >= 5) {
                parse_args(argv, argc, &isPostprocessing, &postprocessingType, &fraction, &isMirror,
                    &isInterp, &isSubpixel, &subScale);                    
            }
        
            dst_image = model->align(src_image, isPostprocessing, postprocessingType, fraction, isMirror, 
                isInterp, isSubpixel, subScale); 
        } 
    } else {
        throw string("unknown action ") + action;
    }
    model->save(dst_image, argv[2]);
    model->send_update();
    }catch(const string& s){
        view->error(s);
        return 1;
    }
    catch (...)
    {
        view->error("unexpected");
        return 1;
    }
    return 0;
}

template<typename ValueType>
ValueType ConsoleControler::read_value(string s)
{
    stringstream ss(s);
    ValueType res;
    ss >> res;
    if (ss.fail() or not ss.eof())
        throw string("bad argument: ") + s;
    return res;
}

template<typename ValueType>
bool ConsoleControler::check_value(string s)
{
    stringstream ss(s);
    ValueType res;
    ss >> res;
    if (ss.fail() or not ss.eof())
        return false;
    return true;
}

template<typename ValueT>
void ConsoleControler::check_number(string val_name, ValueT val, ValueT from, ValueT to)
{
    if (val < from)
        throw val_name + string(" is too small");
    if (val > to)
        throw val_name + string(" is too big");
}

void ConsoleControler::check_argc(int argc, int from, int to)
{
    if (argc < from)
        throw string("too few arguments for operation");

    if (argc > to)
        throw string("too many arguments for operation");
}

Matrix<double> ConsoleControler::parse_kernel(string kernel)
{
    return Matrix<double>(0, 0);
    // Kernel parsing implementation here
}

void ConsoleControler::parse_args(char **argv, int argc, bool *isPostprocessing, string *postprocessingType, double *fraction, bool *isMirror, 
            bool *isInterp, bool *isSubpixel, double *subScale)
{
    for (int i = 4; i < argc; i++) {
        string param(argv[i]);
        
        if (param == "--gray-world" || param == "--unsharp" || 
        param == "--white-balance" || param == "--autocontrast") {
            *isPostprocessing = true;
            *postprocessingType = param;
            if ((param == "--autocontrast") && ((i+1) < argc) && check_value<double>(argv[i+1])) {
                *fraction = read_value<double>(argv[++i]);
            }
        } else if (param == "--subpixel") {
            *isSubpixel = true;
            if (((i+1) < argc) && check_value<double>(argv[i+1])) {
                *subScale = read_value<double>(argv[++i]);
            }
        } else if (param == "--bicubic-interp") {
            *isInterp = true;
        } else if (param == "--mirror") {
            *isMirror = true;
        }else
            throw string("unknown option for --align ") + param;
    }
}