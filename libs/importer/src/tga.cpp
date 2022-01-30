#include "common/Image.hpp"
#include "common/types.hpp"

namespace
{
using namespace common;

constexpr uint8_t TGA_IMAGE_ID = 0x01;
constexpr uint8_t TGA_IMAGE_INFO = 0x02;
constexpr uint8_t TGA_IMAGE_DATA = 0x04;
constexpr uint8_t TGA_COLOR_MAP = 0x08;
/* RLE */
constexpr uint8_t TGA_RLE_ENCODE = 0x10;

/* color format */
constexpr uint8_t TGA_RGB = 0x20;
constexpr uint8_t TGA_BGR = 0x40;

/* orientation */
constexpr uint8_t TGA_BOTTOM = 0x0;
constexpr uint8_t TGA_TOP = 0x1;
constexpr uint8_t TGA_LEFT = 0x0;
constexpr uint8_t TGA_RIGHT = 0x1;

constexpr uint8_t TGA_EOF = 0xff;

#pragma pack(1)
struct TGAHeader
{
    uint8_t IDLength; /* image id length */
    uint8_t ColorMapType; /* color map type */
    uint8_t ImageType; /* image type */
    uint16_t CMapStart; /* index of first map entry */
    uint16_t CMapLength; /* number of entries in color map */
    uint8_t CMapDepth; /* bit-depth of a cmap entry */
    uint16_t XOffset; /* x-coordinate */
    uint16_t YOffset; /* y-coordinate */
    uint16_t Width; /* width of image */
    uint16_t Height; /* height of image */
    uint8_t PixelDepth; /* pixel-depth of image */
    uint8_t ImageDescriptor; /* alpha bits */
};
#pragma pack()

const uint32_t* getColorMap(const uint8_t*& src, uint32_t size, uint8_t depth)
{
    static std::vector<uint32_t> colorMap(256);
    if (colorMap.size() < size) colorMap.resize(size);

    switch (depth)
    {
        case 32: // BGRA
            for (uint32_t i = 0; i < size; ++i)
            {
                colorMap[i] = src[0] << 16 | src[1] << 8 | src[2] | src[3] << 24;
                src += 4;
            }
            break;

        case 24: // BGR
            for (uint32_t i = 0; i < size; ++i)
            {
                colorMap[i] = src[0] << 16 | src[1] << 8 | src[2] | 0xff000000;
                src += 3;
            }
            break;

        case 15: // BGR 5bits per each
            for (uint32_t i = 0; i < size; ++i)
            {
                uint32_t c = src[0] << 8 | src[1];
                colorMap[i] = (c & 0x7c00) << 6 | (c & 0x03e0) << 3 | (c & 0x1f) | 0xff000000;
                src += 2;
            }
            break;

        case 16: // BGRA 5bits for BGR and 1 bit A
            for (uint32_t i = 0; i < size; ++i)
            {
                uint32_t c = src[0] << 8 | src[1];
                colorMap[i] = (c & 0x7c00) << 6 | (c & 0x03e0) << 3 | (c & 0x1f) | ((c & 0x8000) ? 0xff000000 : 0);
                src += 2;
            }
            break;
    }

    return colorMap.data();
}

bool isSupportedTga(const TGAHeader& hdr)
{
    // valid ColorMapType = 0 or 1
    if (hdr.ColorMapType > 1) return false;

    // Compressed color-mapped data, using Huffman, Delta, and runlength encoding is not supported
    if (hdr.ImageType == 0 or (hdr.ImageType >= 4 and hdr.ImageType <= 8) or hdr.ImageType >= 12) return false;

    // bad pixel depth
    if (hdr.PixelDepth != 8 and hdr.PixelDepth != 15 and hdr.PixelDepth != 16 and hdr.PixelDepth != 24 and
        hdr.PixelDepth != 32)
    {
        return false;
    }
    return true;
}
} // namespace

namespace Importer
{
bool DecodeTGA(Image& dst, const MemoryBuffer&& srcMemBuf)
{
    if (srcMemBuf.size() < sizeof(TGAHeader)) return false;

    auto* hdr = reinterpret_cast<const TGAHeader*>(srcMemBuf.data());
    if (not isSupportedTga(*hdr))
    {
        return false;
    }

    dst.createEmpty(hdr->Width, hdr->Height, PixelFormat::R8G8B8A8_UNORM);
    uint8_t* dstBin = dst.editMipmap();
    uint32_t pitch = dst.pitch;
    uint32_t width = dst.width;
    uint32_t height = dst.height;
    uint32_t pixelSize = dst.getPixelSize();

    // real read of TGA
    const uint8_t* s = srcMemBuf.data() + sizeof(TGAHeader) + hdr->IDLength; // ignore imaged ID
    const uint8_t* end_of_src = srcMemBuf.data() + srcMemBuf.size();

    const uint32_t* colorMap = nullptr;
    if (hdr->ImageType == 1 || hdr->ImageType == 9)
    {
        colorMap = getColorMap(s, hdr->CMapLength, hdr->CMapDepth);
    }

    // pixel by pixel
    uint8_t head;
    uint16_t dir = 0, rep = 0;

    // first line
    uint8_t* dstLine = dstBin;
    int dLineStep = pitch;
    int dPixelStep = pixelSize;

    if ((hdr->ImageDescriptor & 0x20))
    { // vertical flip
        dstLine += pitch * (height - 1); // 4*tex.textureWidth*(tex.height-1); // go to last line;
        dLineStep = -dLineStep;
    }

    if (hdr->ImageDescriptor & 0x10)
    { // horizontal flip
        dstLine += (width - 1) * pixelSize;
        dPixelStep = -dPixelStep;
    }

    for (int y = 0; y < hdr->Height; ++y)
    {
        uint8_t* d = dstLine;
        dstLine += dLineStep;
        if (s >= end_of_src) return true;

        uint16_t pixelsInLine = hdr->Width;
        while (pixelsInLine)
        {
            if (hdr->ImageType >= 9 && rep == 0 && dir == 0)
            {
                head = *s++;
                if (head == TGA_EOF) return false; // fail
                if (head >= 128)
                {
                    rep = head - 128;
                    dir = 1;
                }
                else
                {
                    dir = head + 1;
                }
            }
            else
            {
                dir = width; // for uncompresed image
            }

            // dir is always > 0
            // read new pixel
            uint32_t pixel;
            while (dir > 0)
            {
                // read pixel
                switch (hdr->PixelDepth)
                {
                    case 8:
                        pixel = *s++;
                        break;

                    case 15:
                    case 16:
                        pixel = *((uint16_t*)s);
                        s += 2;
                        break;

                    case 24:
                        pixel = 0xff000000 | s[0] << 16 | s[1] << 8 | s[2];
                        s += 3;
                        break;

                    case 32:
                        pixel = s[3] << 24 | s[0] << 16 | s[1] << 8 | s[2];
                        s += 4;
                        break;
                }

                // write pixel
                switch (hdr->ImageType)
                {
                    case 1:
                    case 9: // colormap
                        pixel = colorMap[pixel - hdr->CMapStart];
                        break;

                    case 2:
                    case 10: // 16bit to 32bit or 8->32
                        break;

                    case 3:
                    case 11: // grey
                        pixel = pixel & 0xff;
                        pixel |= (pixel << 24) | (pixel << 16) | (pixel << 8);
                        break;
                }
                *(uint32_t*)d = pixel;
                d += dPixelStep;
                --pixelsInLine;
                --dir;
            }

            // repeat
            int tmpRep = std::min(rep, pixelsInLine);
            rep -= tmpRep;
            pixelsInLine -= tmpRep;
            while (tmpRep > 0)
            {
                --tmpRep;
                *(uint32_t*)d = pixel;
                d += dPixelStep;
            }
        }
    }

    return true;
}
} // namespace Importer

// bool EncodeTGA(TempMemory *dst, Image *src)
// {
// 	if ((src->format != PF_R8G8B8A8_UNORM && src->format != PF_R8G8B8_UNORM) || src->GetPixelSize() != sizeof(uint32_t))
// 		return false;

// 	dst->clear();

// 	TGAHeader *hdr = (TGAHeader *)dst->reserveForNewData(sizeof(TGAHeader));
// 	hdr->IDLength = 0;
// 	hdr->ColorMapType = 0;	// no Color map
// 	hdr->ImageType = 10;	// RLE compresion
// 	hdr->CMapStart = 0;
// 	hdr->CMapLength = 0;
// 	hdr->CMapDepth = 32;
// 	hdr->XOffset = 0;
// 	hdr->YOffset = 0;
// 	hdr->Width = src->width;
// 	hdr->Height = src->height;
// 	hdr->PixelDepth = 32;
// 	hdr->ImageDescriptor = 0x08;

// 	for (int i = (int)src->width - 1; i >= 0; --i) {
// 		uint32_t *s = (uint32_t *)(src->rawImage + i * src->pitch);
// 		uint32_t *e = s + src->width;
// 		while (s < e) {
// 			// check if RLE will do some good
// 			if (s + 1 < e && s[0] == s[1]) { // rle
// 				uint8_t cmd = 0x81;
// 				++s;
// 				while (s + 1 < e && cmd < 0xff && s[0] == s[1]) {
// 					++s;
// 					++cmd;
// 				}

// 				dst->pushData(&cmd, 1);
// 				uint8_t buf[4];
// 				buf[2] = ((uint8_t *)s)[0];
// 				buf[1] = ((uint8_t *)s)[1];
// 				buf[0] = ((uint8_t *)s)[2];
// 				buf[3] = ((uint8_t *)s)[3];
// 				dst->pushData(buf, 4); // 1 pixel rgba
// 				++s;
// 			}
// 			else { // no rle
// 				uint8_t cmd = 0x00;
// 				uint8_t *src = (uint8_t *)s;
// 				++s;
// 				while (cmd < 0x7f && ((s + 1 < e && s[0] != s[1]) || s + 1 == e)) {
// 					++s;
// 					++cmd;
// 				}

// 				dst->pushData(&cmd, 1);
// 				uint8_t buf[4];
// 				uint8_t *ts = src;
// 				for (int i = (cmd & 0x7f) + 1; i > 0; --i) {
// 					buf[2] = ts[0];
// 					buf[1] = ts[1];
// 					buf[0] = ts[2];
// 					buf[3] = ts[3];
// 					ts += 4;
// 					dst->pushData(buf, 4); // 1 pixel rgba
// 				}
// 			}
// 		}
// 	}
// 	/*
// 			// The file footer. This part is totally optional.
// 			static const char footer[ 26 ] =
// 				"\0\0\0\0"  // no extension area
// 				"\0\0\0\0"  // no developer directory
// 				"TRUEVISION-XFILE"  // yep, this is a TGA file
// 				".";
// 			data->PushData((const uint8_t *)footer, sizeof(footer));
// 	*/
// 	return true;
// }
