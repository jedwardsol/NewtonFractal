#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstddef>
#include <utility>
#include <mutex>
#include <cassert>
#include <thread>
#include <chrono>
using namespace std::literals;

#include "bitmap.h"
#include "window.h"


namespace
{


auto makeHeader()
{
    auto header = reinterpret_cast<BITMAPINFO*>( new  std::byte[ sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)]);

    header->bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
    header->bmiHeader.biWidth           =  dim;
    header->bmiHeader.biHeight          = -dim;
    header->bmiHeader.biPlanes          =    1;
    header->bmiHeader.biBitCount        =    8;
    header->bmiHeader.biCompression     = BI_RGB;
    header->bmiHeader.biSizeImage       = dim*dim,
    header->bmiHeader.biXPelsPerMeter   = 0;
    header->bmiHeader.biYPelsPerMeter   = 0;
    header->bmiHeader.biClrUsed         = 0;
    header->bmiHeader.biClrImportant    = 0;


    for(int i=0;i<80;i++)
    {
        BYTE shade = 80+i*2;

        header->bmiColors[i]     = RGBQUAD{ shade,     0,     0};
        header->bmiColors[i+80]  = RGBQUAD{     0, shade,     0};
        header->bmiColors[i+160] = RGBQUAD{     0,     0, shade};
    }


    return header;
}



}

BITMAPINFO     *bitmapHeader        {makeHeader()};
uint8_t         bitmapData[dim][dim]{};
static_assert( (dim%4) == 0);




