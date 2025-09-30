//
// Created by Hayden Chalin on 7/14/25.
//

#ifndef MODEL_H
#define MODEL_H
//#include <Metal/MTLBuffer.hpp>
#include <Metal/Metal.hpp>
#include <tiny_obj_loader.h>
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


/*
 * Material
*/
// TODO get rid of the duplicated Materials
struct Material {
    Eigen::Vector3f ambient{0.2f, 0.2f, 0.2f};
    Eigen::Vector3f diffuse{0.8f, 0.8f, 0.8f};
    Eigen::Vector3f specular{1.0f, 1.0f, 1.0f};
    // Eigen::Vector3f emissive{0.0f, 0.0f, 0.0f};          // Not used rn
    float shininess{32.0f};
    std::string diffuseTexture;
    std::string normalTexture;
};

struct GPUMaterial {
    /**
     * @brief Represents the ambient color of a material.
     *
     * This variable defines the RGB components of the ambient color in a 3D graphics material.
     * The ambient component influences the base color of an object under indirect or ambient lighting
     * conditions, contributing to the object's overall appearance when global lighting is applied.
     *
     * The array is aligned to 16 bytes to ensure optimized memory access when used in GPU operations.
     */
    alignas(16) float ambient[3];  float _pad0{0.f};
    alignas(16) float diffuse[3];  float _pad1{0.f};
    alignas(16) float specular[3]; float shininess{32.f};

    // Later: uint32_t diffuseTexIndex; uint32_t normalTexIndex; uint32_t flags; uint32_t _pad;
};
static_assert(sizeof(GPUMaterial) == 48, "GPUMaterial size mismatch with Metal Material.");
class Model {
public:
    explicit Model(MTL::Device *device, std::string &fileNamme);

    ~Model();

    //@ Useful getters
    MTL::Buffer *getVertexBuffer();
    MTL::Buffer *getIndexBuffer();
    const int getIndexCount();
    Eigen::Matrix4f &getModelMatrix();
    BroMath::Transform &getTransformMatrix();
    MTL::Buffer *getMaterialBuffer() const;


private:
    // @ Members
    int indexCount;                             // # of indices
    std::string &fileName;                       // File name
    MTL::Device *device{nullptr};               // Device
    BroMath::Transform transform; // Old
    Eigen::Matrix4f modelMatrix {Eigen::Matrix4f::Identity()};
    MTL::Texture *vertexTexture{nullptr};       // ptr to texture

    // Buffers
    MTL::Buffer *vertexBuffer;
    MTL::Buffer *indexBuffer;
    std::unordered_map<Vertex, int> uniqueVertices;
    FileType fileType;                           // File type
    MTL::Buffer *materialBuffer{nullptr};
    std::vector<Material> materials;
    std::vector<GPUMaterial> gpuMaterials;          // Specific for GPU

    //@ Methods
    void loadModel();
    void createBuffers(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
    static FileType determineFileType(const std::string &fileName);
    void processMaterials(std::vector<tinyobj::material_t> &materials);
    void createMaterialBuffer();
};



#endif //MODEL_H
