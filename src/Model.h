//
// Created by Hayden Chalin on 7/14/25.
//

#ifndef MODEL_H
#define MODEL_H
//#include <Metal/MTLBuffer.hpp>
#include <Metal/Metal.hpp>
//#include <MetalKit/MetalKit.h> // For MTKTextureLoader

#include <fstream>
#include "common/common.h"
#include <iostream>
#include "common/BroMath/Transform.h"


enum struct FileType {
    OBJ,
    GLTF,
    GLB,
    FBX
};



class Model {
public:
    explicit Model(MTL::Device *device, std::string &fileNamme);

    ~Model();

    //@ Useful getters
    MTL::Buffer *getVertexBuffer();
    MTL::Buffer *getIndexBuffer();
    const int getIndexCount();
    BroMath::Transform &getTransform();


private:
    // @ Members
    int indexCount;                             // # of indices
    std::string &fileName;                       // File name
    MTL::Device *device{nullptr};               // Device
    BroMath::Transform transform;                      // ! Namespace error right now
    MTL::Texture *vertexTexture{nullptr};       // ptr to texture
    // Buffers
    MTL::Buffer *vertexBuffer;
    MTL::Buffer *indexBuffer;
    std::unordered_map<Vertex, int> uniqueVertices;
    FileType fileType;                           // File type

    //@ Methods
    void loadModel();
    void createBuffers(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
    static FileType determineFileType(const std::string &fileName);
};



#endif //MODEL_H
