set(MODULE_NAME renderingEngine)

set(SOURCES 
    src/renderingEngine.cpp
    src/glfw.cpp
    src/glfwImpl.hpp
    src/glfwImpl.cpp
    src/outputWindow.cpp
)

set(HEADERS 
    include/renderingEngine/renderingEngine.hpp
    include/renderingEngine/glfw.hpp
    include/renderingEngine/outputWindow.hpp
)

add_compile_definitions(GLFW_INCLUDE_VULKAN)

add_library(${MODULE_NAME} ${SOURCES} ${HEADERS}) 
add_library(bps::${MODULE_NAME} ALIAS ${MODULE_NAME})
target_include_directories(${MODULE_NAME} PUBLIC include)

target_link_libraries(${MODULE_NAME} 
    PUBLIC
        # bps::config              # exposed as public, so ut don't need to add it in target_link_libraries
        bps::common

    PRIVATE
        # SDL2::SDL2_image
        glfw
    )

add_subdirectory(ut)