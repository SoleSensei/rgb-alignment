/*median filter*/
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

Plugin::Plugin(): name("median") {}

class Median
{
    Image pic;
    void quickSort(int arr[], int left, int right) const;
public:
    const int radius;
    Median(const Image& src, const int r): pic(src), radius(r) {}
    std::tuple<uint,uint,uint> operator()(const Image& neighbourhood) const;
};

tuple<uint,uint,uint> Median::operator()(const Image& neighbourhood) const
{
    const auto size = 2*radius+1;
    const int dsize = size*size;
    /*//==========Qsort==========
    int Rb[dsize],Gb[dsize],Bb[dsize]; //brightness
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            auto p = size*i+j;
            std::tie(Rb[p],Gb[p],Bb[p])  = neighbourhood(i,j);
        }
    }

    quickSort(Rb,0,dsize-1);
    quickSort(Gb,0,dsize-1);
    quickSort(Bb,0,dsize-1);
    
    auto rPix = Rb[radius*size+radius];
    auto gPix = Gb[radius*size+radius];
    auto bPix = Bb[radius*size+radius];
    
    return std::make_tuple(rPix,gPix,bPix);
    */ //==========Histogram==========
    int r_cl,g_cl,b_cl;
    int rHist[256],gHist[256],bHist[256];
    for (int i = 0; i < 256; ++i) rHist[i]=gHist[i]=bHist[i]=0; //zeroing
    //built rgb histograms
    for (int i = 0; i < size; ++i)
        for(int j = 0; j < size; ++j){
            std::tie(r_cl,g_cl,b_cl) = neighbourhood(i,j);
            rHist[r_cl]++;
            gHist[g_cl]++;
            bHist[b_cl]++;
        }
    auto sum_r = 0, sum_g = 0, sum_b = 0;
    int r,g,b;
    for (int i = 0; i < 256; ++i) //find median
    {
        sum_r += rHist[i];
        sum_g += gHist[i];
        sum_b += bHist[i];
        if (sum_r > dsize/2){ //found median
            r = i;
            sum_r = 0;
        }
        if (sum_g > dsize/2){
            g = i;
            sum_g = 0;
        }
        if (sum_b > dsize/2){
            b = i;
            sum_b = 0;
        }
    }

    return std::make_tuple(r,g,b);
        
}

void Median::quickSort(int arr[], int left, int right) const
{
    int i = left, j = right;
    int mid = (left + right) / 2;
    int pivot = arr[mid];
    
    while (i <= j){
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;
        if (i <= j)
            std::swap(arr[i++], arr[j--]);
    };

    if (left < j)
        quickSort(arr, left, j);
    if (i < right)
        quickSort(arr, i, right);
}

Image Plugin::processing(Image src_image, const int radius, double fr) {
    cerr << "median " << radius << endl;
    src_image = mirror(src_image, radius);
    src_image = src_image.unary_map(Median(src_image,radius));
 
    return mirror_crop(src_image, radius); 
}