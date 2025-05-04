#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <Metal/Metal.hpp>
#include "readShaderFile.h"

std::string readShaderFile(const std::string &targetFileName)
{

    std::string fileName;
    {
        namespace fs = std::filesystem;
        fs::path startPath = fs::current_path().parent_path();

        for (auto &p : fs::recursive_directory_iterator(startPath))
        {
            if (p.path().filename() == targetFileName)
            {
                fileName = p.path().string();
                break;
            }
        }
    }

    std::ifstream file(fileName);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open shader file: " + fileName);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// void loadShaderFromFile(MTL::Device *device, const std::string &filePath)
void loadShaderFromFile(MTL::Library *&inLib, MTL::Device *device, const std::string &fileName)
{
    std::string shaderSource = readShaderFile(fileName);

    if (shaderSource.empty())
        std::runtime_error("Shaders file is empty");

    NS::String *source = NS::String::string(shaderSource.c_str(), NS::UTF8StringEncoding);
    NS::Error *error = nullptr;
    MTL::CompileOptions *compileOptions = MTL::CompileOptions::alloc()->init();
    MTL::Library *library = device->newLibrary(source, compileOptions, &error);
    // MTL::Library *library = device->newLibrary(source, &error); // doesn't work??
    compileOptions->release();

    if (!library)
    {
        if (error)
        {
            std::cerr << "Failed to compile Metal library: " << error->localizedDescription()->utf8String() << std::endl;
        }
        else
        {
            std::cerr << "Failed to compile Metal library: Unknown error" << std::endl;
        }
        return;
    }
    else
    {
        inLib = library;
    }
}
