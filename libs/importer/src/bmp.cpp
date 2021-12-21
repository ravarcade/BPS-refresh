#include "common/types.hpp"

int version()
{
    return 1;
}

// using namespace BAMS;

// #pragma pack(2)

// typedef struct
// {
// 	WORD    bfType;        // must be 'BM'
// 	DWORD   bfSize;        // size of the whole .bmp file
// 	WORD    bfReserved1;   // must be 0
// 	WORD    bfReserved2;   // must be 0
// 	DWORD   bfOffBits;
// } SBITMAPFILEHEADER;

// typedef struct
// {
// 	DWORD  biSize;            // size of the structure
// 	LONG   biWidth;           // image width
// 	LONG   biHeight;          // image height
// 	WORD   biPlanes;          // bitplanes
// 	WORD   biBitCount;        // resolution
// 	DWORD  biCompression;     // compression
// 	DWORD  biSizeImage;       // size of the image
// 	LONG   biXPelsPerMeter;   // pixels per meter X
// 	LONG   biYPelsPerMeter;   // pixels per meter Y
// 	DWORD  biClrUsed;         // colors used
// 	DWORD  biClrImportant;    // important colors
// } SBITMAPINFOHEADER;

// #pragma pack()

// /**
// * Decode TGA to RGBx texture
// */
// bool DecodeBMP(Image *dst, uint8_t *src, size_t size, TempMemory &tmp)
// {
// 	if (size < sizeof(BITMAPFILEHEADER))
// 		return false;

// 	SBITMAPFILEHEADER *hdr = reinterpret_cast<SBITMAPFILEHEADER *>(src);
// 	if (hdr->bfType != 0x4d42) // check if file start with "BM"
// 		return false;

// 	SBITMAPINFOHEADER *inf = reinterpret_cast<SBITMAPINFOHEADER *>(src + sizeof(SBITMAPFILEHEADER));

// 	LONG tempHeight = inf->biHeight;
// 	bool vflip = tempHeight < 0;
// 	if (vflip)
// 		tempHeight = -tempHeight;

// 	dst->CreateEmpty(inf->biWidth, inf->biHeight, PF_R8G8B8A8_UNORM);
// 	BYTE *dstBin = dst->rawImage;
// 	unsigned int pitch = dst->pitch;
// 	unsigned int width = dst->width;
// 	unsigned int height = dst->height;

// 	BYTE *s = reinterpret_cast<BYTE *>(src + sizeof(SBITMAPFILEHEADER) + sizeof(SBITMAPINFOHEADER));

// 	static DWORD32 colorMap[256];
// 	if (inf->biBitCount < 24) { // color pallete
// 		BYTE *cm = (BYTE *)colorMap;
// 		for (int colors = 1 << inf->biBitCount; colors > 0; --colors) {
// 			*cm++ = *s++;
// 			*cm++ = *s++;
// 			*cm++ = *s++;
// 			*cm++ = 255;  ++s;
// 		}
// 	}

// 	unsigned int rowsizefix = ((32 - inf->biBitCount*width) >> 3) & 0x3;

// 	switch (inf->biBitCount) {
// 	case 8:
// 		if (inf->biCompression != 0)
// 			return false;
// 		for (int y = height - 1; y >= 0; --y) {
// 			DWORD32 *d = (DWORD32 *)(dstBin + pitch * y);
// 			for (int x = width - 1; x >= 0; --x) {
// 				*d++ = colorMap[*s++];
// 			}
// 			s += rowsizefix;
// 		}
// 		break;

// 	case 24:
// 		if (inf->biCompression != 0)
// 			return false;
// 		for (unsigned int y = 0; y < height; ++y) {
// 			BYTE *d = dstBin + pitch * (vflip ? height - y + 1 : y);
// 			for (int x = width - 1; x >= 0; --x) {
// 				*d++ = s[2];
// 				*d++ = s[1];
// 				*d++ = s[0];
// 				*d++ = 255;
// 				s += 3;
// 			}
// 			s += rowsizefix;
// 		}
// 		break;

// 	case 32:
// 		if (inf->biCompression != 0)
// 			return false;
// 		for (unsigned int y = 0; y < height; ++y) {
// 			BYTE *d = dstBin + pitch * (vflip ? height - y + 1 : y);
// 			for (int x = width - 1; x >= 0; --x) {
// 				*d++ = *s++;
// 				*d++ = *s++;
// 				*d++ = *s++;
// 				*d++ = 255;
// 				++s;
// 			}
// 			//				s += rowsizefix; // rowsizefix must be 0.
// 		}
// 		break;
// 	}

// 	return true;
// }
