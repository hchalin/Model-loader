//
// Created by Hayden Chalin on 7/14/25.
//
#define TINYOBJLOADER_IMPLEMENTATION

#include <filesystem>
#include "Model.h"

#include <__ranges/transform_view.h>


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

Model::~Model() {
    if (vertexBuffer) {
        vertexBuffer->release();
        vertexBuffer = nullptr;
    }
    if (indexBuffer) {
        indexBuffer->release();
        indexBuffer = nullptr;
    }

    if (vertexTexture) {
        vertexTexture->release();
        vertexTexture = nullptr;
    }
    if (materialBuffer) {
        materialBuffer->release();
        materialBuffer = nullptr;
    }
}

BroMath::Transform &Model::getTransformMatrix() {
    return this->transform;
}


Eigen::Matrix4f &Model::getModelMatrix(){
    return modelMatrix;
}



FileType Model::determineFileType(const std::string& fileName) {
    size_t dotPos = fileName.find_last_of('.');
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
    std::vector<uint32_t> indices;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err, warn;

    // std::string assetPath = std::filesystem::current_path().string() + "/../src/assets/" + fileName;
    std::filesystem::path objPath  = std::filesystem::current_path() / ".." / "src" / "assets" / fileName;
    std::filesystem::path baseDir = objPath.parent_path();

    // std::cout << "Loading model from: " << objPath << std::endl;

    bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.string().c_str(), baseDir.string().c_str(),
                                /*triangulate*/ true);


    if (!ok) {
        throw std::runtime_error("Failed to load OBJ file: " + warn + err);
    }
    
    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }

    // Process materials
    processMaterials(materials);

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    // Process all shapes and their faces
    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        
        // Process each face in the shape
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            size_t fv = shape.mesh.num_face_vertices[f];

            // Get material index for this face
            int materialId = shape.mesh.material_ids[f];

            // Process each vertex in the face (usually 3 for triangles)
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                Vertex vertex{};

                // Position (required)
                if (idx.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2],
                        1.0f
                    };

                    // @ Color support - Not being used
                    vertex.color = {
                        attrib.colors[3 * idx.vertex_index + 0],
                        attrib.colors[3 * idx.vertex_index + 1],
                        attrib.colors[3 * idx.vertex_index + 2],
                        1.0f
                    };

                }

                // Add material to vertex
                vertex.materialIndex = (materialId >= 0) ? materialId : 0;

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

                //vertices.push_back(vertex);
                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);         // If the vertex is new, add
                }
                    indices.push_back(uniqueVertices[vertex]);
            }
            
            index_offset += fv;
        }
    }

    // std::cout << "Processed " << vertices.size() << " vertices" << std::endl;
    // std::cout << "Processed " << indices.size() << " indices" << std::endl;
    indexCount = indices.size();

    // Create Metal buffers
    createBuffers(vertices, indices);
    createMaterialBuffer();
}


void Model::processMaterials(std::vector<tinyobj::material_t> &objMaterials) {
    // clear the materias vector and reserve correct size
    materials.clear();
    materials.reserve(objMaterials.size());

    for (const auto& mat: objMaterials) {
        Material material{};
        material.ambient = {mat.ambient[0], mat.ambient[1], mat.ambient[2]};
        material.diffuse = {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]};
        // std::cout <<  "r: " <<  material.diffuse[0] << std::endl;
        // std::cout << "g: " << material.diffuse[1] << std::endl;
        // std::cout << "b: " << material.diffuse[2] << std::endl;
        material.specular = {mat.specular[0], mat.specular[1], mat.specular[2]};
        material.shininess = mat.shininess;
        material.diffuseTexture = mat.diffuse_texname;
        material.normalTexture = mat.normal_texname;

        materials.push_back(material);;
    }

    if (materials.empty())
        materials.push_back(Material{});
    // std::cout << "Loaded " << materials.size() << " materials" << std::endl;

}
void Model::createMaterialBuffer() {
    std::vector<GPUMaterial> gpu;
    gpu.reserve(materials.size());
    for (auto& m : materials) {
        GPUMaterial g{};
        g.ambient[0] = m.ambient.x(); g.ambient[1] = m.ambient.y(); g.ambient[2] = m.ambient.z();
        g.diffuse[0] = m.diffuse.x(); g.diffuse[1] = m.diffuse.y(); g.diffuse[2] = m.diffuse.z();
        g.specular[0] = m.specular.x(); g.specular[1] = m.specular.y(); g.specular[2] = m.specular.z();
        g.shininess = m.shininess;
        gpu.push_back(g);
    }
    if (gpu.empty()) return;
    if (materialBuffer) { materialBuffer->release(); materialBuffer = nullptr; }
    materialBuffer = device->newBuffer(gpu.data(),
                                       gpu.size() * sizeof(GPUMaterial),
                                       MTL::ResourceStorageModeManaged);
    materialBuffer->didModifyRange(NS::Range(0, gpu.size() * sizeof(GPUMaterial)));
}
MTL::Buffer *Model::getMaterialBuffer() const {
    return materialBuffer;
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
