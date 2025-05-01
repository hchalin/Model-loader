cmake_minimum_required(VERSION 3.30)
project(Model_Viewer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")




add_executable(Model_Viewer
        src/main.cpp
        src/Window.cpp
        src/Window.h
        src/Renderer.cpp
        src/Renderer.h
)
# Add the Metal-cpp implementation files
target_sources(Model_Viewer PRIVATE
        ${CMAKE_SOURCE_DIR}/Dependencies/Foundation/NSPrivate.hpp
        ${CMAKE_SOURCE_DIR}/Dependencies/QuartzCore/CAPrivate.hpp
        ${CMAKE_SOURCE_DIR}/Dependencies/Metal/MTLPrivate.hpp
)
find_package(glfw3 REQUIRED)

# Single consolidated include directories call
target_include_directories(Model_Viewer PRIVATE
        ${CMAKE_SOURCE_DIR}/Dependencies/       # When including directories, reload the project.
        ${GLFW_INCLUDE_DIRS}
        /usr/local/include  # Add Homebrew include path explicitly
        SYSTEM /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks
)
# Link GLFW library
target_link_libraries(Model_Viewer PRIVATE glfw)

target_include_directories(Model_Viewer
        PRIVATE
        Dependencies
)

find_library(METAL Metal)
find_library(FOUNDATION Foundation)
find_library(QUARTZCORE QuartzCore)
find_library(APPKIT AppKit)
#find_library(ACCELERATE Accelerate)

target_link_libraries(Model_Viewer PRIVATE
        stdc++
        "-framework Metal"
        "-framework MetalKit"
        "-framework ModelIO"
        "-framework Foundation"
        "-framework QuartzCore"
        #"-framework Accelerate"
        "-framework AppKit" objc
)