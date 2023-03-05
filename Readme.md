# List of installed packages (with vcpkg)

```
fmt:x64-mingw-dynamic                             9.1.0#1             Formatting library for C++. It can be used as a ...
glfw3:x64-mingw-dynamic                           3.3.8#2             GLFW is a free, Open Source, multi-platform libr...
glm:x64-mingw-dynamic                             0.9.9.8#2           OpenGL Mathematics (GLM)
glslang:x64-mingw-dynamic                         11.13.0             Khronos reference front-end for GLSL and ESSL, a...
glslang[tools]:x64-mingw-dynamic                                      Build the glslangValidator and spirv-remap binaries
gtest:x64-mingw-dynamic                           1.13.0              GoogleTest and GoogleMock testing frameworks       
imgui:x64-mingw-dynamic                           1.89.3              Bloat-free Immediate Mode Graphical User interfa...
imgui[glfw-binding]:x64-mingw-dynamic                                 Make available GLFW binding
imgui[vulkan-binding]:x64-mingw-dynamic                               Make available Vulkan binding
libjpeg-turbo:x64-mingw-dynamic                   2.1.5.1             libjpeg-turbo is a JPEG image codec that uses SI...
libpng:x64-mingw-dynamic                          1.6.39              libpng is a library implementing an interface fo...
libwebp:x64-mingw-dynamic                         1.3.0               WebP codec: library to encode and decode images ...
libwebp[libwebpmux]:x64-mingw-dynamic                                 Build the libwebpmux library
libwebp[nearlossless]:x64-mingw-dynamic                               Enable near-lossless encoding
libwebp[simd]:x64-mingw-dynamic                                       Enable any SIMD optimization.
libwebp[unicode]:x64-mingw-dynamic                                    Build Unicode executables. (Adds definition UNIC...
sdl2-image:x64-mingw-dynamic                      2.6.3               SDL_image is an image file loading library. It l...
sdl2-image[libjpeg-turbo]:x64-mingw-dynamic                           Support for JPEG image format
sdl2-image[libwebp]:x64-mingw-dynamic                                 Support for WEBP image format.
sdl2:x64-mingw-dynamic                            2.26.3#1            Simple DirectMedia Layer is a cross-platform dev...
sdl2[base]:x64-mingw-dynamic                                          Base functionality for SDL
spirv-cross:x64-mingw-dynamic                     2021-01-15#2        SPIRV-Cross is a practical tool and library for ...
spirv-headers:x64-mingw-dynamic                   1.3.231.1#1         Machine-readable files for the SPIR-V Registry     
spirv-reflect:x64-mingw-dynamic                   1.3.236.0           SPIRV-Reflect is a lightweight library that prov...
sqlite3:x64-mingw-dynamic                         3.40.1#1            SQLite is a software library that implements a s...
vcpkg-cmake-config:x64-mingw-dynamic              2022-02-06#1        
vcpkg-cmake-config:x64-windows                    2022-02-06#1        
vcpkg-cmake:x64-windows                           2022-12-22
vulkan-headers:x64-mingw-dynamic                  1.3.239             Vulkan header files and API registry
vulkan:x64-mingw-dynamic                          1.1.82.1#3          A graphics and compute API that provides high-ef...
zlib:x64-mingw-dynamic                            1.2.13              A compression library
```

# Vulkan SDK: 1.3.239.0

    find_path(SPIRV_HEADERS_INCLUDE_DIRS "spirv/1.0/GLSL.std.450.h")
    target_include_directories(main PRIVATE ${SPIRV_HEADERS_INCLUDE_DIRS})


    
    # this is heuristically generated, and may not be correct
    find_package(spirv_cross_c CONFIG REQUIRED)
    target_link_libraries(main PRIVATE spirv-cross-c)

    find_package(spirv_cross_core CONFIG REQUIRED)
    target_link_libraries(main PRIVATE spirv-cross-core)

    find_package(spirv_cross_cpp CONFIG REQUIRED)
    target_link_libraries(main PRIVATE spirv-cross-cpp)

    find_package(spirv_cross_glsl CONFIG REQUIRED)
    target_link_libraries(main PRIVATE spirv-cross-glsl)

    find_package(spirv_cross_hlsl CONFIG REQUIRED)
    target_link_libraries(main PRIVATE spirv-cross-hlsl)

    find_package(spirv_cross_msl CONFIG REQUIRED)
    target_link_libraries(main PRIVATE spirv-cross-msl)

    find_package(spirv_cross_reflect CONFIG REQUIRED)
    target_link_libraries(main PRIVATE spirv-cross-reflect)

    find_package(spirv_cross_util CONFIG REQUIRED)
    target_link_libraries(main PRIVATE spirv-cross-util)


-- spirv-reflect
# this is heuristically generated, and may not be correct
    find_package(unofficial-spirv-reflect CONFIG REQUIRED)
    target_link_libraries(main PRIVATE unofficial::spirv-reflect::spirv-reflect)


    find_package(glslang CONFIG REQUIRED)
    target_link_libraries(main PRIVATE 
    glslang::OSDependent 
    glslang::glslang 
    glslang::MachineIndependent 
    glslang::GenericCodeGen 
    glslang::OGLCompiler 
    glslang::glslangValidator 
    glslang::spirv-remap 
    glslang::glslang-default-resource-limits 
    glslang::SPVRemapper 
    glslang::SPIRV 
    glslang::HLSL)