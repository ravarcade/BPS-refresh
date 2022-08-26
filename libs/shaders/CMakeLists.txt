set(MODULE_NAME shaders)

set(HEADERS 
    include/shaders/ShaderCompiler.hpp
    include/shaders/ShaderReflections.hpp
    include/shaders/ShaderVariable.hpp
)

set(SOURCES 
    src/ShaderCompiler.cpp
    src/ShaderReflections.cpp
    src/ShaderVariable.cpp
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
        # glfw
        glm::glm
        imgui::imgui
        Vulkan::Vulkan
        spirv-cross-core
        spirv-cross-glsl
        spirv-cross-reflect
        # HLSL
        SPIRV
        glslang
        # OGLCompiler
    )

add_subdirectory(ut)