#include <assert.h>

#include "shiftCalc.hpp"

using namespace std;


tuple<uint,uint,uint> overflow_crop(const tuple<uint,uint,uint>& pix)
{   
    int r,g,b;
    std::tie(r,g,b) = pix;

    r = r < 0 ? 0 : (r > 255 ? 255 : r);
    g = g < 0 ? 0 : (g > 255 ? 255 : g); 
    b = b < 0 ? 0 : (b > 255 ? 255 : b); 
                 
    return std::make_tuple(r,g,b);

}

tuple <ssize_t,ssize_t,ssize_t,ssize_t> setBound (ssize_t ver, ssize_t hor,
                                                  const optShift& sh1,
                                                  const optShift& sh2,
                                                  const Image& pic)
{
    ssize_t rows = int( pic.n_rows-1 );
    ssize_t cols = int( pic.n_cols-1 );

    ssize_t vs1 = ((ver - sh1.ver) < 0) ? 0 : ver - sh1.ver;
    vs1 = vs1 > rows ? rows : vs1;
    ssize_t hs1 = ((hor - sh1.hor) < 0) ? 0 : hor - sh1.hor;
    hs1 = hs1 > cols ? cols : hs1;
    ssize_t vs2 = ((ver - sh2.ver) < 0) ? 0 : ver - sh2.ver;
    vs2 = vs2 > rows ? rows : vs2;
    ssize_t hs2 = ((hor - sh2.hor) < 0) ? 0 : hor - sh2.hor;
    hs2 = hs2 > cols ? cols : hs2;

    return std::make_tuple(vs1, hs1, vs2, hs2);
}

Image imposition(const optShift& GR_align,
                 const optShift& GB_align,
                 Image& r, Image& g, Image& b)
{

    for (ssize_t i = 0; i < g.n_rows; i++) {
        for (ssize_t j = 0; j < g.n_cols; j++){ 
            ssize_t v1,h1,v2,h2;
            std::tie(v1,h1,v2,h2) = setBound(i,j,GR_align,GB_align,g);
            g(i, j) = std::make_tuple(std::get<0>(r(v1, h1)), 
                                      std::get<0>(g(i, j)),
                                      std::get<0>(b(v2, h2)));
        }
    }
    return g;
}

Image imposition_cut(const Image& pic,
                     const optShift& sh1,
                     const optShift& sh2)
{
    uint prow = std::max({ssize_t(0), sh1.ver, sh2.ver}); 
    uint pcol = std::max({ssize_t(0), sh1.hor, sh2.hor}); 
    uint srow = std::min({ssize_t(pic.n_rows), pic.n_rows + sh1.ver, pic.n_rows + sh2.ver});
    uint scol = std::min({ssize_t(pic.n_cols), pic.n_cols + sh1.hor, pic.n_cols + sh2.hor});
   
    return pic.submatrix(prow,pcol,srow-prow,scol-pcol); 
}

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


optShift optimalAlign(const Image& fixed,
                      const Image& moved,
                      optShift opt, 
                      ssize_t limit)
{
    if (limit == 0) return opt;
    auto shift_v = opt.ver;
    auto shift_h = opt.hor;
    for(ssize_t y = shift_v-limit; y <= shift_v+limit; ++y) {
        for(ssize_t x = shift_h-limit; x <= shift_h+limit; ++x)
        {
            Image crop_fixed, crop_moved; //croped images after shift
            tie(crop_fixed, crop_moved) = shiftCrop(fixed, moved, y, x);
            //=====MES======
            auto metric = calc_rms(crop_fixed, crop_moved);
            //=====crossCorrelation=======
            // auto metric = calc_crossCorrelation(crop_fixed, crop_moved);
            // if (metric >= opt.metric) {
            if (metric <= opt.metric) {
                opt.metric = metric;
                opt.hor = x;
                opt.ver = y;
            }
        }
    }

    return opt;
}

tuple<Image,Image> shiftCrop(const Image& fixed,
                             const Image& moved,
                             ssize_t vS, //shifts
                             ssize_t hS)
{
    using std::max;
    uint fromX,fromY,lenX,lenY;
    fromX = max(ssize_t(0), hS); //cols
    fromY = max(ssize_t(0), vS); //rows
    lenX = max(ssize_t(0), fixed.n_cols - abs(hS));
    lenY = max(ssize_t(0), fixed.n_rows - abs(vS));

    Image crop_fixed = fixed.submatrix(fromY, fromX, lenY, lenX);

    fromX = max(ssize_t(0), -hS);
    fromY = max(ssize_t(0), -vS);

    Image crop_moved = moved.submatrix(fromY, fromX, lenY, lenX);

    return tuple<Image,Image>{crop_fixed, crop_moved};
}

unsigned long long calc_rms(const Image& img1,
                            const Image& img2)
{
    unsigned long long res = 0;
    auto crop_r = 0.08 * img1.n_rows; //crop 8% from borders
    auto crop_c = 0.08 * img1.n_cols;
    for (uint i = crop_r; i < img1.n_rows - crop_r; i++) {
        for (uint j = crop_c; j < img1.n_cols - crop_c; j++) {
            auto basis = ssize_t( get<0>(img1(i, j)) ) - get<0>(img2(i, j));
            res += pow(basis, 2);
        }
    }
    assert( (img1.n_cols * img1.n_rows) != 0 ); //stop if multiply == 0
    return res / (img1.n_cols * img1.n_rows);
}

unsigned long long calc_crossCorrelation(const Image& img1,
                                         const Image& img2)
{
    unsigned long long res = 0;
    auto crop_r = 0.08 * img1.n_rows; //crop 8% from borders
    auto crop_c = 0.08 * img1.n_cols;
    for (uint i = crop_r; i < img1.n_rows - crop_r; i++) {
        for (uint j = crop_c; j < img1.n_cols - crop_c; j++) {
            res += get<0>(img1(i, j)) * get<0>(img2(i, j));
        }
    }
    return res;
}

Pyramida calc_pyramid(Pyramida pyr) {
    pyr.level++;
    double scale = 2;
    Image oldPyr = pyr.res; 
    if (pyr.res.n_rows > 900) {
        pyr.res = calc_scale(pyr.res,1/scale);
        pyr = calc_pyramid(pyr);
    }

    auto part = oldPyr.n_rows / 3;
    Image b = oldPyr.submatrix(0,0,part,oldPyr.n_cols);
    Image g = oldPyr.submatrix(part,0,part,oldPyr.n_cols);
    Image r = oldPyr.submatrix(part * 2,0,part,oldPyr.n_cols);

    pyr.GR = optimalAlign(g, r, pyr.GR, pyr.limit);
    pyr.GB = optimalAlign(g, b, pyr.GB, pyr.limit);
    if (pyr.level == 1) 
        pyr.res = imposition(pyr.GR, pyr.GB, r, g, b);

    pyr.limit = pyr.limit * (1/scale);
    pyr.GR = pyr.GR * scale;
    pyr.GB = pyr.GB * scale;
    pyr.level--;

    return pyr;
}

Image mirror(const Image& src, const int radius)
{
    std::cerr<<"mirror"<<std::endl;
    Image res{src.n_rows+2*radius, src.n_cols+2*radius};
    for(ssize_t i = 0; i < res.n_rows; ++i)
        for(ssize_t j = 0; j < res.n_cols; ++j){
            auto x = i >= src.n_rows + radius ? 2*(src.n_rows-1)+radius - i : abs(radius - i);  
            auto y = j >= src.n_cols + radius ? 2*(src.n_cols-1)+radius - j : abs(radius - j);  
            res(i,j) = src(x,y);
        }

    return res;
}

Image mirror_crop(const Image& src, const int radius)
{       
    Image res{src.n_rows-2*radius, src.n_cols-2*radius};
    for(ssize_t i = radius; i < src.n_rows-radius; ++i)
        for(ssize_t j = radius; j < src.n_cols-radius; ++j)
            res(i-radius,j-radius) = src(i,j);
    return res;
}

tuple <uint,uint,uint> unaryOp::operator()(const Image& neighbourhood) const
{
    uint r,g,b;
    auto sr = 0, sg = 0, sb = 0;
    auto borders = 2*radius+1;
    for(uint i = 0; i < borders; ++i)
        for(uint j = 0; j < borders; ++j)
        {
            std::tie(r,g,b) = neighbourhood(i,j);
            sr += r * kernel(i,j);
            sg += g * kernel(i,j); 
            sb += b * kernel(i,j); 
        }
    
    return overflow_crop(std::make_tuple(sr,sg,sb));
} 

Image calc_scale(const Image& src, const double scale)
{
    double x1,y1,x2,y2;
    double frd, fgr, fbl;
    double r1, g1, b1;    
    double r2, g2, b2;    
    double r3, g3, b3;    
    double r4, g4, b4;
    Image res(src.n_rows * scale, src.n_cols * scale);    
    
    for (uint i = 0; i < uint(src.n_rows * scale); i++)
        for (uint j = 0; j < uint(src.n_cols * scale); j++) {
            x1 = i >= scale * (src.n_rows - 1) ? src.n_rows - 2 : i / scale;
            y1 = j >= scale * (src.n_cols - 1) ? src.n_cols - 2 : j / scale;
            x2 = x1 + 1;
            y2 = y1 + 1;
            
            double fracX = x1 - int(x1);
            double fracY = y1 - int(y1);
            
            std::tie(r1,g1,b1) = src(x1,y1);
            std::tie(r2,g2,b2) = src(x2,y1);
            std::tie(r3,g3,b3) = src(x1,y2);
            std::tie(r4,g4,b4) = src(x2,y2);

            frd = (r1 * (1 - fracX) + r2 * fracX)*(1 - fracY) + (r3*(1 - fracX) + r4*fracX)*fracY;
            fgr = (g1 * (1 - fracX) + g2 * fracX)*(1 - fracY) + (g3*(1 - fracX) + g4*fracX)*fracY;
            fbl = (b1 * (1 - fracX) + b2 * fracX)*(1 - fracY) + (b3*(1 - fracX) + b4*fracX)*fracY;

            frd = frd < 0 ? 0 : ( frd > 255 ? 255 : frd);
            fgr = fgr < 0 ? 0 : ( fgr > 255 ? 255 : fgr); 
            fbl = fbl < 0 ? 0 : ( fbl > 255 ? 255 : fbl); 

            res(i,j) = make_tuple(frd,fgr,fbl);

        }
    return res;
}
