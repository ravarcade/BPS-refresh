#pragma once

#include "common/types.hpp"

namespace common
{
enum PixelFormat
{
    UNKNOWN = 0,
    R32G32B32A32_SFLOAT = 1,
    R8G8B8A8_UNORM = 2,
    R8G8B8_UNORM = 3,
    BC2_UNORM = 4,
};

class Image
{
public:
    Image();
    Image(
        const MemoryBuffer memBuf,
        uint32_t width,
        uint32_t height = 0,
        PixelFormat format = PixelFormat::R8G8B8A8_UNORM,
        uint32_t pitch = 0);

    Image(const Image&) = delete;
    Image(Image&&) = delete;
    Image& operator =(const Image&) = delete;
    Image& operator =(Image&&) = delete;

    MemoryBuffer getMipmap(uint32_t mipmap = 0, uint32_t layer = 0, uint32_t face = 0) const;
    size_t getOffset(uint32_t mipmap = 0, uint32_t layer = 0, uint32_t face = 0) const;
    MemoryBuffer getRawImage() const;
    uint8_t* editMipmap(uint32_t mipmap = 0, uint32_t layer = 0, uint32_t face = 0);
    void createEmpty(
        uint32_t width,
        uint32_t height,
        PixelFormat format = PixelFormat::R8G8B8A8_UNORM,
        uint32_t pitch = 0);

    void createFull(
        uint32_t width,
        uint32_t height,
        PixelFormat format,
        void* data,
        size_t dataLen,
        size_t* offsets = nullptr,
        uint32_t num_mipmaps = 1,
        uint32_t num_layers = 1,
        uint32_t num_faces = 1);


    uint32_t width;
    uint32_t pitch;
    uint32_t height;
    PixelFormat format;

    // cubemap & array textures support
    uint32_t num_layers;
    uint32_t num_faces;
    uint32_t num_mipmaps;
    bool isCube;
    bool isArray;

    bool isLoaded() { return rawImage != nullptr; }

    uint32_t getPixelSize() { return getPixelSize(format); }

    static uint32_t getPixelSize(PixelFormat format);
    static uint32_t getLineAlign(PixelFormat format);

    bool operator ==(const Image& ) const;
private:
    std::unique_ptr<uint8_t[]> buffer;
    std::vector<uint8_t*> rawImages;
    uint8_t* rawImage;
};
} // namespace common