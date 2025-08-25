cmake_minimum_required(VERSION 3.15)
project(Model_Viewer VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
#    add_link_options(-fsanitize=address,undefined)
#endif()

# Define source and header files
set(SOURCE_FILES
        src/main.cpp
        src/Window.cpp
        src/Renderer.cpp
        src/Camera.cpp
        src/Model.cpp
        src/backend/glfw_adaptor.mm
        src/shaders/readShaderFile.cpp
        src/common/BroMath/Transform.cpp
        tests/test_main.cpp
        tests/test_camera.cpp
        src/Scene.cpp
)
set(HEADER_FILES
        src/common/BroMath/vec4.h
        src/common/BroMath/Transform.h
        src/Camera.h
        src/Scene.h
        src/Model.h
        src/backend/tiny_obj_loader.h
)

# Add executable
add_executable(Model_Viewer ${SOURCE_FILES} ${HEADER_FILES})


# Find GLFW
find_package(glfw3 REQUIRED)

# JSON
find_package(jsoncpp CONFIG REQUIRED)


# Include directories
target_include_directories(Model_Viewer PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/eigen        # Includes Eigen (and other headers if needed)
        ${GLFW_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src/backend
        SYSTEM /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks
)
# Link GLFW library
target_link_libraries(Model_Viewer PRIVATE glfw)
#Json
target_link_libraries(Model_Viewer PRIVATE JsonCpp::JsonCpp)

target_include_directories(Model_Viewer
        PRIVATE
        Dependencies
)

find_library(METAL Metal)
find_library(FOUNDATION Foundation)
find_library(QUARTZCORE QuartzCore)
find_library(METALKIT MetalKit)
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
message(STATUS "MetalKit library: ${METALKIT}")


## TESTING
# Find Google Test
find_package(GTest REQUIRED)

# Add test executable
add_executable(Model_Viewer_Tests
        tests/test_main.cpp
        tests/test_camera.cpp
)

# Link Google Test libraries
target_link_libraries(Model_Viewer_Tests PRIVATE
        GTest::GTest
        GTest::Main
)

# Include directories for tests
target_include_directories(Model_Viewer_Tests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

#JSON`