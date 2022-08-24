#include "common/Image.hpp"
#include <cassert>
#include <cstring>

namespace
{
using namespace common;

struct
{
    uint32_t size;
    uint32_t lineAllign;
} pixelDescription[] = {
    {0, 0}, // PF_UNKNOWN
    {16, 4}, // PF_R32G32B32A32_SFLOAT
    {4, 4}, // PF_R8G8B8A8_UNORM
    {4, 4}, // PF_R8G8B8_UNORM
    {4, 4}, // PF_R8G8B8A8_UNORM
};

uint32_t getPitch(PixelFormat format, uint32_t width)
{
    auto la = Image::getLineAlign(format);
    return (width * Image::getPixelSize(format) + la - 1) & (0 - la);
}
} // namespace

namespace common
{
Image::Image()
    : width{0}
    , pitch{0}
    , height{0}
    , format{PixelFormat::UNKNOWN}
    , num_layers{1}
    , num_faces{1}
    , num_mipmaps{1}
    , isCube{false}
    , isArray{false}
    , buffer{nullptr}
    , rawImages{}
    , rawImage{nullptr}
{
}

Image::Image(MemoryBuffer _memBuf, uint32_t _width, uint32_t _height, PixelFormat _format, uint32_t _pitch)
    : width{_width}
    , pitch{_pitch ? _pitch : getPitch(_format, _width)}
    , height{_height and pitch ? _height : static_cast<uint32_t>(_memBuf.size() / pitch)}
    , format{_format}
    , num_layers{1}
    , num_faces{1}
    , num_mipmaps{1}
    , isCube{false}
    , isArray{false}
    , buffer{std::make_unique<uint8_t[]>(pitch * height)}
    , rawImages{{buffer.get(), buffer.get() + pitch * height}}
    , rawImage{rawImages[0]}
{
    memcpy_s(rawImages[0], rawImages[1] - rawImages[0], _memBuf.data(), _memBuf.size());
}

MemoryBuffer Image::getMipmap(uint32_t mipmapLevel, uint32_t layer, uint32_t face) const
{
    if (mipmapLevel < num_mipmaps and layer < num_layers and face < num_faces and rawImages.size() != 0)
    {
        size_t idx = face + layer * num_faces + mipmapLevel * num_faces * num_layers;
        return {rawImages[idx], static_cast<size_t>(rawImages[idx + 1] - rawImages[idx])};
    }
    return {};
}

size_t Image::getOffset(uint32_t mipmapLevel, uint32_t layer, uint32_t face) const
{
    if (mipmapLevel < num_mipmaps and layer < num_layers and face < num_faces and rawImages.size() != 0)
    {
        size_t idx = face + layer * num_faces + mipmapLevel * num_faces * num_layers;
        return rawImages[idx] - rawImages[0];
    }
    return 0;
}

MemoryBuffer Image::getRawImage() const
{
    return {*rawImages.begin(), static_cast<size_t>(*rawImages.rbegin() - *rawImages.begin())};
}

uint8_t* Image::editMipmap(uint32_t mipmapLevel, uint32_t layer, uint32_t face)
{
    assert(mipmapLevel < num_mipmaps and layer < num_layers and face < num_faces and rawImages.size() != 0);
    size_t idx = face + layer * num_faces + mipmapLevel * num_faces * num_layers;
    return rawImages[idx];
}

void Image::createEmpty(uint32_t width, uint32_t height, PixelFormat format, uint32_t _pitch)
{
    pitch = _pitch ? _pitch : getPitch(format, width);
    isCube = false;
    isArray = false;
    createFull(width, height, format, nullptr, pitch * height);
}

void Image::createFull(
    uint32_t _width,
    uint32_t _height,
    PixelFormat _format,
    void* data,
    size_t dataLen,
    size_t* offsets,
    uint32_t _num_mipmaps,
    uint32_t _num_layers,
    uint32_t _num_faces)
{
    width = _width;
    height = _height;
    format = _format;
    num_mipmaps = _num_mipmaps;
    num_layers = _num_layers;
    num_faces = _num_faces;
    isCube = false;

    size_t numRawImages = num_mipmaps * num_layers * num_faces;
    rawImages.resize(numRawImages + 1);
    buffer = std::make_unique<uint8_t[]>(dataLen);
    rawImage = buffer.get();

    if (data)
    {
        memcpy_s(buffer.get(), dataLen, data, dataLen);
    }
    if (offsets)
    {
        for (size_t i = 0; i < numRawImages; ++i)
        {
            rawImages[i] = rawImage + offsets[i];
        }
    }
    else
    {
        size_t offset = 0;

        uint32_t w = width;
        uint32_t h = height;
        for (uint32_t mip = 0; mip < num_mipmaps; ++mip)
        {
            auto la = getLineAlign(format);
            size_t pitch = (w * getPixelSize(format) + la - 1) & (0 - la);
            size_t iSize = pitch * h;
            iSize = (iSize + 3) & -4L;

            for (uint32_t layer = 0; layer < num_layers; ++layer)
            {
                for (uint32_t face = 0; face < num_faces; ++face)
                {
                    size_t idx = face + layer * this->num_faces + mip * this->num_faces * this->num_layers;
                    rawImages[idx] = buffer.get() + offset;
                    offset += iSize;
                }
            }
            w = w > 1 ? w >> 1 : 1;
            h = h > 1 ? h >> 1 : 1;
        }
    }
    *rawImages.rbegin() = rawImage + dataLen;
}

uint32_t Image::getPixelSize(PixelFormat format)
{
    return pixelDescription[format].size;
}

uint32_t Image::getLineAlign(PixelFormat format)
{
    return pixelDescription[format].lineAllign;
}

bool Image::operator==(const Image& b) const
{
    return width == b.width and height == b.height and format == b.format and pitch == b.pitch and
        num_layers == b.num_layers and num_faces == b.num_faces and num_mipmaps == b.num_mipmaps and
        isCube == b.isCube and isArray == b.isArray and
        ((rawImage == nullptr and b.rawImage == nullptr) or
         (rawImage != nullptr and b.rawImage != nullptr and
          memcmp(rawImage, b.rawImage, *rawImages.rbegin() - *rawImages.begin()) == 0));
}
} // namespace common
