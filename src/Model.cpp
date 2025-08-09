//
// Created by Hayden Chalin on 7/14/25.
//
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <filesystem>
#include "Model.h"



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
    // @ This code has been written with the help of AI REVIEW IT

    // ! this is not efficient, it is not using indexed vertices

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err, warn;

    std::string assetPath = std::filesystem::current_path().string() + "/../src/assets/" + fileName;
    std::cout << "Loading model from: " << assetPath << std::endl;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, assetPath.c_str())) {
        throw std::runtime_error("Failed to load OBJ file: " + warn + err);
    }
    
    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }

    //std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
    //std::cout << "# of shapes    : " << shapes.size() << std::endl;

    // Process all shapes and their faces
    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        
        // Process each face in the shape
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = shape.mesh.num_face_vertices[f];
            
            // Process each vertex in the face (usually 3 for triangles)
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                
                Vertex vertex;
                
                // Position (required)
                if (idx.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2],
                        1.0f
                    };
                }
                
                // Normal (if available)
                if (idx.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    };
                } else {
                    // Default normal pointing up
                    vertex.normal = {0.0f, 1.0f, 0.0f};
                }
                
                // Texture coordinates (if available)
                if (idx.texcoord_index >= 0) {
                    vertex.texCoord = {
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * idx.texcoord_index + 1] // Flip Y for Metal
                    };
                } else {
                    vertex.texCoord = {0.0f, 0.0f};
                }
                
                vertices.push_back(vertex);
                indices.push_back(static_cast<uint32_t>(vertices.size() - 1));
            }
            
            index_offset += fv;
        }
    }

    std::cout << "Processed " << vertices.size() << " vertices" << std::endl;
    std::cout << "Processed " << indices.size() << " indices" << std::endl;
    indexCount = indices.size();

    // Create Metal buffers
    createBuffers(vertices, indices);
}

const int Model::getIndexCount() {
    if (indexCount == 0) {
        throw std::invalid_argument("Index count is null");
    }
    return indexCount;
}

void Model::createBuffers(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) {
    if (vertices.size() == 0) {
        throw std::invalid_argument("No vertices provided");
    }

    if (indices.size() == 0) {
        throw std::invalid_argument("No indices provided");
    }

    vertexBuffer = device->newBuffer(vertices.data(), vertices.size() * sizeof(Vertex), MTL::ResourceStorageModeManaged);
    if (!vertexBuffer) {
        throw std::runtime_error("Failed to create vertex buffer");
    }

    indexBuffer  = device->newBuffer(indices.data(), indices.size() * sizeof(uint32_t), MTL::ResourceStorageModeManaged);
    if (!indexBuffer) {
        throw std::runtime_error("Failed to create index buffer");
    }
}

MTL::Buffer * Model::getVertexBuffer() {
    if (!vertexBuffer) {
        throw std::runtime_error("Failed to get vertex buffer for model");
    }
    return vertexBuffer;
}

MTL::Buffer * Model::getIndexBuffer() {
   if (!indexBuffer) {
       throw std::runtime_error("Failed to get index buffer for model");
   }
    return indexBuffer;
}
