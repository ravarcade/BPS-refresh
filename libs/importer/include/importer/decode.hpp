#include "common/Image.hpp"

namespace Importer
{
bool DecodeSDL(common::Image& dst, const MemoryBuffer&& src, const char *imgType = nullptr);
} // namespace Importer