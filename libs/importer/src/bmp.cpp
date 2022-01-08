#include "common/Image.hpp"
#include "common/types.hpp"


namespace
{
using namespace common;

const uint16_t BM_TAG = 0x4d42;

#pragma pack(2)
typedef struct
{
    int16_t bfType; // must be 'BM'
    uint32_t bfSize; // size of the whole .bmp file
    int16_t bfReserved1; // must be 0
    int16_t bfReserved2; // must be 0
    uint32_t bfOffBits;
} SBITMAPFILEHEADER;

typedef struct
{
    uint32_t biSize; // size of the structure
    int32_t biWidth; // image width
    int32_t biHeight; // image height
    int16_t biPlanes; // bitplanes
    int16_t biBitCount; // resolution
    uint32_t biCompression; // compression
    uint32_t biSizeImage; // size of the image
    int32_t biXPelsPerMeter; // pixels per meter X
    int32_t biYPelsPerMeter; // pixels per meter Y
    uint32_t biClrUsed; // colors used
    uint32_t biClrImportant; // important colors
} SBITMAPINFOHEADER;
#pragma pack()

uint32_t *getColorMap(const uint8_t *&src, uint32_t size)
{
    static uint32_t colorMap[256];
    uint8_t* cm = (uint8_t*)colorMap;
    for (; size > 0; --size)
    {
        *cm++ = src[2];
        *cm++ = src[1];
        *cm++ = src[0];
        *cm++ = 255;
        src += 4;
    }

    return colorMap;
}

} // namespace

namespace Importer
{
bool DecodeBMP(Image& dst, const MemoryBuffer &&srcMemBuf)
{
    if (srcMemBuf.size() < sizeof(SBITMAPFILEHEADER)) return false;

    auto hdr = reinterpret_cast<const SBITMAPFILEHEADER*>(srcMemBuf.data());
    auto inf = reinterpret_cast<const SBITMAPINFOHEADER*>(srcMemBuf.data() + sizeof(SBITMAPFILEHEADER));
    auto* src = srcMemBuf.data() + sizeof(SBITMAPFILEHEADER) + sizeof(SBITMAPINFOHEADER);

    if (hdr->bfType != BM_TAG or inf->biCompression != 0) return false;

    int32_t tempHeight = inf->biHeight;
    bool vflip = tempHeight < 0;
    if (vflip) tempHeight = -tempHeight;

    dst.createEmpty(inf->biWidth, inf->biHeight);
    uint8_t* dstBin = dst.editMipmap();
    uint32_t pitch = dst.pitch;
    uint32_t width = dst.width;
    uint32_t height = dst.height;
    uint32_t rowsizefix = ((32 - inf->biBitCount * width) >> 3) & 0x3;

    switch (inf->biBitCount)
    {
        case 4:
        {
            auto colorMap = getColorMap(
                src, inf->biClrUsed ? std::min(1u << inf->biBitCount, inf->biClrUsed) : 1 << inf->biBitCount);
            for (uint32_t y = 0; y < height; ++y)
            {
                uint32_t* d = reinterpret_cast<uint32_t*>(dstBin + pitch * (vflip ? height - y - 1 : y));
                for (uint32_t x = 0; x < width; ++x)
                {
                    if (x&1)
                    {
                        *d++ = colorMap[*src & 0xf];
                        ++src;
                    } else  
                    {
                        *d++ = colorMap[*src >> 4];
                    }
                }
                src += rowsizefix;
            }
        }
            break;

        case 8:
        {
            auto colorMap = getColorMap(
                src, inf->biClrUsed ? std::min(1u << inf->biBitCount, inf->biClrUsed) : 1 << inf->biBitCount);
            for (uint32_t y = 0; y < height; ++y)
            {
                uint32_t* d = reinterpret_cast<uint32_t*>(dstBin + pitch * (vflip ? height - y - 1 : y));
                for (uint32_t x = 0; x < width; ++x)
                {
                    *d++ = colorMap[*src++];
                }
                src += rowsizefix;
            }
        }
            break;

        case 24:
            for (uint32_t y = 0; y < height; ++y)
            {
                uint8_t* d = dstBin + pitch * (vflip ? height - y - 1 : y);
                for (uint32_t x = 0; x < width; ++x)
                {
                    *d++ = src[2];
                    *d++ = src[1];
                    *d++ = src[0];
                    *d++ = 255;
                    src += 3;
                }
                src += rowsizefix;
            }
            break;

        case 32:
            for (uint32_t y = 0; y < height; ++y)
            {
                uint8_t* d = dstBin + pitch * (vflip ? height - y - 1 : y);
                for (uint32_t x = 0; x < width; ++x)
                {
                    *d++ = *src++;
                    *d++ = *src++;
                    *d++ = *src++;
                    *d++ = 255;
                    ++src;
                }
            }
            break;
    }

    return true;
}
} // namespace Importer
