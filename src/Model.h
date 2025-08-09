//
// Created by Hayden Chalin on 7/14/25.
//

#ifndef MODEL_H
#define MODEL_H
//#include <Metal/MTLBuffer.hpp>
#include <Metal/Metal.hpp>
#include <fstream>
#include <json/json.h>
#include "common/common.h"
#include <iostream>


enum struct FileType {
    OBJ,
    GLTF,
    GLB,
    FBX
};

// @ Used to store the face index information
// ^ f v/vt/vn
struct FaceInfo {
    int vertexIndex;
    int texcoordIndex;          // Not used yet
    int vertexNormalIndex;      // Not used yet
};

class Model {
public:
    explicit Model( MTL::Device* device, std::string& fileNamme);
    ~Model() = default;

    MTL::Buffer * getVertexBuffer();
    MTL::Buffer * getIndexBuffer();
    const int getIndexCount();

    private:
    std::string& fileName;
    MTL::Device * device{nullptr};

    int indexCount;

    // Buffers
    MTL::Buffer* vertexBuffer;
    MTL::Buffer* indexBuffer;

    //std::unordered_map<Vertex, int> uniqueVertices;
    void loadModel();
    void createBuffers(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

    FileType fileType;

    static FileType determineFileType(const std::string& fileName);






};



#endif //MODEL_H
