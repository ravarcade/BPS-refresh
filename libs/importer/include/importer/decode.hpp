#include "common/Image.hpp"

namespace Importer
{
bool DecodeSDL(common::Image& dst, MemoryBuffer src, const char *imgType = nullptr);
} // namespace Importer