set(MODULE_NAME engine)
set(SOURCES 
    src/dummy.cpp
)

set(HEADERS 
    include/engine/dummy.hpp
)

add_library(${MODULE_NAME} ${SOURCES} ${HEADERS})
add_library(bps::${MODULE_NAME} ALIAS ${MODULE_NAME})
target_include_directories(${MODULE_NAME} PUBLIC include)

target_link_libraries(${MODULE_NAME} 
    PUBLIC
        # bps::config              # exposed as public, so ut don't need to add it in target_link_libraries
        bps::common
    PRIVATE 
        )
