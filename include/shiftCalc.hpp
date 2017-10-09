#pragma once

#include "io.h"

using namespace std;

struct optShift 
{
    uint64_t metric;
    ssize_t hor;   
    ssize_t ver;
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


tuple<uint,uint,uint> overflow_crop(const std::tuple<uint,uint,uint>& pix);

Image imposition_cut(const Image& pic,
    const optShift& RG,
    const optShift& RB);

tuple <ssize_t,ssize_t,ssize_t,ssize_t> setBound (ssize_t ver, ssize_t hor,
                                                  const optShift& gr, 
                                                  const optShift& bl,
                                                  const Image& r);
optShift optimalAlign(const Image& fixed,
                      const Image& moved,
                      ssize_t limit);

tuple<Image,Image> shiftCrop(const Image& fixed,  
                             const Image& moved,
                             ssize_t vS, //shifts
                             ssize_t hS);

unsigned long long calc_rms(const Image& img1,
                            const Image& img2);

unsigned long long calc_crossCorrelation(const Image& img1,
                           const Image& img2);

Image mirror (const Image& src, const int radius);
Image mirror_crop(const Image& src, const int radius);
