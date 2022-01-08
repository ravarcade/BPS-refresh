set(MODULE_NAME common)

set(SOURCES 
    src/base64.cpp
    src/Image.cpp
    src/MemoryBuffer.cpp
)

set(HEADERS 
    include/common/types.hpp
    # include/common/MemoryBuffer.hpp
    include/tools/base64.hpp
    include/common/Image.hpp
)

add_library(${MODULE_NAME} STATIC ${SOURCES} ${HEADERS}) 
target_include_directories(${MODULE_NAME} PUBLIC include)
add_library(bps::${MODULE_NAME} ALIAS ${MODULE_NAME})

add_subdirectory(ut)