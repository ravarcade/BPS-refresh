#include "common/Image.hpp"
#include "common/types.hpp"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL_image.h>

namespace
{
using namespace common;
using RwopDeleter = decltype(
    [](SDL_RWops* res)
    {
        if (res) SDL_FreeRW(res);
    });

using SurfaceDeleter = decltype(
    [](SDL_Surface* res)
    {
        if (res) SDL_FreeSurface(res);
    });

auto createSafeSwop(const MemoryBuffer& srcMemBuf)
{
    return std::unique_ptr<SDL_RWops, RwopDeleter>(SDL_RWFromConstMem(srcMemBuf.data(), srcMemBuf.size()));
}

auto loadImg(auto& rwop, const char *imgType = nullptr)
{
    return imgType == nullptr
    ? std::unique_ptr<SDL_Surface, SurfaceDeleter>(IMG_Load_RW(rwop.get(), 0))
    : std::unique_ptr<SDL_Surface, SurfaceDeleter>(IMG_LoadTyped_RW(rwop.get(), 0, imgType));
}
auto convertImg(auto& img, Uint32 format)
{
    return std::unique_ptr<SDL_Surface, SurfaceDeleter>(SDL_ConvertSurfaceFormat(img.get(), format, 0));
}

bool initSdl()
{
    static bool isInitialized = false;
    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;
        int flags = IMG_INIT_PNG | IMG_INIT_JPG;
        int initted = IMG_Init(flags);
        isInitialized = (initted & flags) == flags;
    }
    return isInitialized;
}
} // namespace

namespace Importer
{
bool DecodeSDL(Image& dst, const MemoryBuffer&& srcMemBuf, const char *imgType)
{
    if (not initSdl()) return false;

    auto rwop = createSafeSwop(srcMemBuf);
    auto img = loadImg(rwop, imgType);
    if (not img.get()) return false;

    img = convertImg(img, SDL_PIXELFORMAT_ABGR8888);
    if (not img.get()) return false;

    dst.createEmpty(img->w, img->h);
    uint8_t* dstBin = dst.editMipmap();
    uint8_t* srcBin = reinterpret_cast<uint8_t*>(img->pixels);

    // flip up/down
    for (uint32_t i = 0; i < dst.height; ++i)
    {
        memcpy_s(dstBin + (dst.height - i - 1) * dst.pitch, dst.pitch, srcBin + i * img->pitch, img->pitch);
    }

    return true;
}
} // namespace Importer
