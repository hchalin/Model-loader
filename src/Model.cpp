//
// Created by Hayden Chalin on 7/14/25.
//
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <filesystem>
#include "Model.h"
#include "common/common.h"



Model::Model(MTL::Device* device, std::string &fileNamme): device(device),fileName(fileNamme),
                                                           vertexBuffer(nullptr), indexBuffer(nullptr)
{
   if (fileNamme == "") throw std::invalid_argument("File name missing");

    // Determin fileName fileformat
    fileType = determineFileType(fileNamme);

    // Parse data
    switch (fileType) {
       case  (FileType::OBJ):
           //parseObj(fileName);
           break;
        default:
            throw std::invalid_argument("File type missing");
    }

    if (!device) {
        throw std::invalid_argument("Device is null");
    }

    loadModel();

}




FileType Model::determineFileType(const std::string& fileName) {
    size_t dotPos = fileName.find_last_of('.');
    size_t gPos = fileName.find_last_of('g');
    std::cout << "G pos: " << gPos/8 << std::endl;
    if (dotPos == std::string::npos) {
        throw std::invalid_argument("File name does not have an extension");
    }

    std::string extension = fileName.substr(dotPos + 1);
    if (extension == "obj") {
        return FileType::OBJ;
    }else if (extension == "gltf") {
        return FileType::GLTF;
    } else if (extension == "glb") {
        return FileType::GLB;
    } else if (extension == "fbx") {
        return FileType::FBX;
    } else {
        throw std::invalid_argument("File type missing");
    }

}

void Model::loadModel() {
    // @ This function will use tiny obj
    std::vector<Vertex> vertices;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err, warn;

    std::string assetPath = std::filesystem::current_path().string() + "/../src/assets/" + fileName;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, assetPath.c_str())) {
        throw std::runtime_error(warn + err);
    }
    std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex;
            if (index.vertex_index >= 0) {
                vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2],
                1.0
                };

            }
            std::cout << "Index: " << index.vertex_index << std::endl;
            std::cout << "Vertex: " << vertex.position.x() << ", " << vertex.position.y() << ", " << vertex.position.z() << std::endl;
            vertices.push_back(vertex);
        }
    }

    for (const auto&shape : shapes) {
        for (const auto& index : shape.mesh.indices) {

        }
    }

    for (const auto vertex : vertices) {
        //std::cout << vertex.position.x() << " " << vertex.position.y() << " " << vertex.position.z() << std::endl;
    }

}

MTL::Buffer * Model::getVertexBuffer() {
    if (!vertexBuffer) {
        throw std::runtime_error("Failed to get vertex buffer for model");
    }
    return vertexBuffer;
}
