set(MODULE_NAME renderingEngine)

set(HEADERS 
    include/renderingEngine/glfw.hpp
    include/renderingEngine/renderingEngine.hpp
)

set(SOURCES 
    src/glfw.cpp
    src/renderingEngine.cpp
    src/impl/CommandBuffers.cpp
    src/impl/CommandBuffers.hpp
    src/impl/CommandPools.cpp
    src/impl/CommandPools.hpp
    src/impl/Context.cpp
    src/impl/Context.hpp
    src/impl/ContextTools.cpp
    src/impl/DefferedRenderPass.cpp
    src/impl/DefferedRenderPass.hpp
    src/impl/DescriptorSetManager.cpp
    src/impl/DescriptorSetManager.hpp
    src/impl/ForwardRenderPass.cpp
    src/impl/ForwardRenderPass.hpp
    src/impl/FrameBufferAttachment.cpp
    src/impl/FrameBufferAttachment.hpp
    src/impl/GlfwImpl.cpp
    src/impl/GlfwImpl.hpp
    src/impl/Gui.cpp
    src/impl/Gui.hpp
    src/impl/IRenderingEngine.hpp
    src/impl/LogicalDevice.cpp
    src/impl/LogicalDevice.hpp
    src/impl/OutputWindow.cpp
    src/impl/OutputWindow.hpp
    src/impl/PhysicalDevice.cpp
    src/impl/PhysicalDevice.hpp
    src/impl/PipelineStatistic.cpp
    src/impl/PipelineStatistic.hpp
    src/impl/QueueFamilyIndices.cpp
    src/impl/QueueFamilyIndices.hpp
    src/impl/RenderingEngineImpl.cpp
    src/impl/RenderingEngineImpl.hpp
    src/impl/RenderPass.hpp
    src/impl/Semaphores.cpp
    src/impl/Semaphores.hpp
    src/impl/SharedUniformBufferObject.cpp
    src/impl/SharedUniformBufferObject.hpp
    src/impl/Surface.cpp
    src/impl/Surface.hpp
    src/impl/SwapChain.cpp
    src/impl/SwapChain.hpp
    src/impl/SwapChainSupportDetails.cpp
    src/impl/SwapChainSupportDetails.hpp
    src/impl/Texture.hpp
    src/impl/Texture2D.cpp
    src/impl/Texture2D.hpp
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
        glm::glm
        imgui::imgui
        Vulkan::Vulkan
    )

add_subdirectory(ut)