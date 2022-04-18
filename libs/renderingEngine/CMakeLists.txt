set(MODULE_NAME renderingEngine)

set(HEADERS 
    include/renderingEngine/glfw.hpp
    include/renderingEngine/renderingEngine.hpp
)

set(SOURCES 
    src/glfw.cpp
    src/renderingEngine.cpp
    src/impl/ire.hpp
    src/impl/GlfwImpl.cpp
    src/impl/GlfwImpl.hpp
    src/impl/LogicalDevice.hpp
    src/impl/LogicalDevice.cpp
    src/impl/PhysicalDevice.hpp
    src/impl/PhysicalDevice.hpp
    src/impl/PhysicalDevice.cpp
    src/impl/OutputWindowImpl.cpp
    src/impl/OutputWindowImpl.hpp
    src/impl/QueueFamilyIndices.cpp
    src/impl/QueueFamilyIndices.hpp
    src/impl/RenderingEngineImpl.cpp
    src/impl/RenderingEngineImpl.hpp
    src/impl/SwapChainSupportDetails.cpp
    src/impl/SwapChainSupportDetails.hpp
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
        Vulkan::Vulkan
    )

add_subdirectory(ut)