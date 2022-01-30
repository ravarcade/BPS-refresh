#include "common/Image.hpp"

namespace Importer
{
bool DecodeBMP(common::Image& dst, const MemoryBuffer&& src);
bool DecodeTGA(common::Image& dst, const MemoryBuffer&& src);
bool DecodeSDL(common::Image& dst, const MemoryBuffer&& src, const char *imgType = nullptr);
} // namespace Importer