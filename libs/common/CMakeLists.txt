set(MODULE_NAME common)

set(SOURCES 
    src/dummy.cpp
)

set(HEADERS 
    include/common/dummy.hpp
    include/common/types.hpp
)

add_library(${MODULE_NAME} ${SOURCES} ${HEADERS}) 
target_include_directories(${MODULE_NAME} PUBLIC include)
add_library(bps::${MODULE_NAME} ALIAS ${MODULE_NAME})