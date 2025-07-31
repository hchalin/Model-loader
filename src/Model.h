//
// Created by Hayden Chalin on 7/14/25.
//

#ifndef MODEL_H
#define MODEL_H
//#include <Metal/MTLBuffer.hpp>
#include <Metal/Metal.hpp>
#include <fstream>
#include <json/json.h>
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

    private:
    std::string& fileName;
    MTL::Device * device{nullptr};
    // Buffers
    MTL::Buffer* vertexBuffer;
    MTL::Buffer* indexBuffer;

    FileType fileType;

    static FileType determineFileType(const std::string& fileName);

    void loadModel();





};



#endif //MODEL_H
