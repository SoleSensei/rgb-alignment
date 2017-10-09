#pragma once
#include <climits>
#include "io.h"

const unsigned long long varmax = ULLONG_MAX;


struct optShift 
{
    uint64_t metric;
    ssize_t hor;   
    ssize_t ver;
    optShift(): metric(varmax), hor(0), ver(0){}
    
    optShift& operator= (const optShift& s)
    {
        hor = s.hor;
        ver = s.ver;
        metric = s.metric;
        return *this;
    }

    optShift operator*(double d)
    {
        hor *= d;
        ver *= d;
        return *this;
    }
};
struct Pyramida 
{
    Image res;
    optShift GR;
    optShift GB;
    int limit;    
    int level;

    Pyramida(const Image& src, optShift r, optShift b, int lim = 20): res(src), GR(r), GB(b), limit(lim), level(0) {}
};


class unaryOp
{
    Matrix<double> kernel;
public:
    const uint radius;
    unaryOp(const Matrix<double>& ker): kernel(ker), radius((ker.n_rows-1)/2) {}
    std::tuple<uint,uint,uint> operator()(const Image& neighbourhood) const;

};

class Median
{
    Image pic;
    void quickSort(int arr[], int left, int right) const;
public:
    const int radius;
    Median(const Image& src, const int r): pic(src), radius(r) {}
    std::tuple<uint,uint,uint> operator()(const Image& neighbourhood) const;
};


std::tuple<uint,uint,uint> overflow_crop(const std::tuple<uint,uint,uint>& pix);

std::tuple <ssize_t,ssize_t,ssize_t,ssize_t> setBound (ssize_t ver, ssize_t hor,
    const optShift& gr, 
    const optShift& bl,
    const Image& r);

Image imposition(const optShift& GR_align,
                 const optShift& GB_align,
                 Image& r, Image& g, Image& b);

Image imposition_cut(const Image& pic,
                     const optShift& RG,
                     const optShift& RB);


optShift optimalAlign(const Image& fixed,
                      const Image& moved,
                      optShift opt,
                      ssize_t limit);

std::tuple<Image,Image> shiftCrop(const Image& fixed,  
                             const Image& moved,
                             ssize_t vS, //shifts
                             ssize_t hS);

unsigned long long calc_rms(const Image& img1,
                            const Image& img2);

unsigned long long calc_crossCorrelation(const Image& img1,
                           const Image& img2);

Pyramida calc_pyramid(Pyramida pyr);

Image mirror (const Image& src, const int radius);
Image mirror_crop(const Image& src, const int radius);

Image calc_scale(const Image& src, const double scale);