set(MODULE_NAME common)

set(SOURCES
    src/base64.cpp
    src/Image.cpp
    src/MemoryBuffer.cpp
    src/Logger.cpp
)

set(HEADERS
    include/common/enumFormatter.hpp
    include/common/Image.hpp
    include/common/Logger.hpp
    include/common/MemoryBuffer.hpp
    include/common/Rect2D.hpp
    include/common/types.hpp
    include/tools/base64.hpp
    include/tools/contains.hpp
    include/tools/countBits.hpp
    include/tools/to_vector.hpp
    include/tools/writeFile.hpp
)

add_library(${MODULE_NAME} STATIC ${SOURCES} ${HEADERS})
add_library(bps::${MODULE_NAME} ALIAS ${MODULE_NAME})

target_include_directories(${MODULE_NAME} PUBLIC include)
target_link_libraries(${MODULE_NAME}
    PUBLIC
        fmt::fmt
    PRIVATE
)

add_subdirectory(ut)