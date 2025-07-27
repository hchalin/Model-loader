//
// Created by Hayden Chalin on 7/14/25.
//

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
           parseObj(fileName);
           break;
        default:
            throw std::invalid_argument("File type missing");
    }

    if (!device) {
        throw std::invalid_argument("Device is null");
    }

}

void Model::loadModelGLB(MTL::Device * device) {
    /**
     * Loads a model via a .glb binary file
     * https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#glb-file-format-specification
     */

    // ! THIS IS NOT COMPLETELY IMPLEMENTED
    fileName = "../src/assets/" + fileName;
    std::cout << "Loading model from file " << fileName << std::endl;
    std::ifstream binFile(fileName, std::ios::binary);
    if (!binFile.is_open()) {
        throw std::runtime_error("Failed to open binary file: " + fileName);
    }

    binFile.seekg(12); // Skip past the 12-byte header to the JSON header
    uint32_t jsonLength;
    binFile.read(reinterpret_cast<char*>(&jsonLength), sizeof(uint32_t)); // Read JSON length

    std::string jsonStr;
    jsonStr.resize(jsonLength);
    binFile.seekg(20); // Skip the rest of the JSON header to the start of the string
    binFile.read(jsonStr.data(), jsonLength); // Read JSON string

    Json::Value _json;
    Json::Reader reader;
    if (!reader.parse(jsonStr, _json)) {
        std::cerr << "Problem parsing assetData: " << jsonStr << std::endl;
        return;
    }

    uint32_t binLength;
    binFile.read(reinterpret_cast<char*>(&binLength), sizeof(binLength)); // Read binary length
    binFile.seekg(sizeof(uint32_t), std::ios_base::cur); // Skip chunk type

    std::vector<char> bin(binLength);
    binFile.read(bin.data(), binLength);

    // Process the binary data and JSON as needed
    Json::Value& primitive = _json["meshes"][0]["primitives"][0];
    Json::Value& positionAccessor = _json["accessors"][primitive["attributes"]["POSITION"].asInt()];
    Json::Value& bufferView = _json["bufferViews"][positionAccessor["bufferView"].asInt()];

    float* buffer = reinterpret_cast<float*>(bin.data() + bufferView["byteOffset"].asInt());
    for (int i = 0; i < positionAccessor["count"].asInt(); ++i) {
        std::cout << "(" << buffer[i * 3] << ", " << buffer[i * 3 + 1] << ", " << buffer[i * 3 + 2] << ")" << std::endl;
    }

    std::cout << "vertices: " << positionAccessor["count"].asInt() << std::endl;
    // Extract position data pointer and size
    int vertexCount = positionAccessor["count"].asInt();
    int vertexByteLength = vertexCount * 3 * sizeof(float); // 3 floats per vertex (x, y, z)

    // Create the Metal buffer from the binary data
    if (!device) {
        throw std::runtime_error("Device not set");
    }

    vertexBuffer = device->newBuffer(buffer, vertexByteLength, MTL::ResourceStorageModeManaged);
    if (!vertexBuffer) {
        throw std::runtime_error("Failed to create vertex buffer for model");
    }
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

void Model::parseObj(const std::string& fileName) {
    // @ Find and open file
    std::fstream objFile("../src/assets/" +fileName);
    if (!objFile.is_open()) {
        throw std::invalid_argument("Failed to open file: " + fileName);
    }


    // Vertice Array
    std::vector<Vertex> vertices;

    // @ Start parsing the file
    std::string lineStr;

    std::string line;
    while (std::getline(objFile, line)) {
        std::size_t pos = line.find(' ');   // Position of the first white space in each line
        std::string lineType= (pos == std::string::npos)   // no space found?
                                ? line                        //    take whole line
                                : line.substr(0, pos);

        // @ vertex
        if( lineType == "v" )
        {
            // x
            std::stringstream lineSS(line.substr(pos + 1)); // Use a stringstream to parse the line
            float vert1, vert2, vert3;
            lineSS >> vert1 >> vert2 >> vert3; // Extract vertex coordinates from the stringstream
            Vertex vertice = {
                {vert1, vert2, vert3, 1.0},
                {1.0, 0.0, 0.0, 1.0}
            };
            vertices.push_back(vertice);
        }

        if (lineType == "f") {
            std::string face;
            std::stringstream lineSS(line.substr(pos + 1));

            lineSS >> face;
            std::cout << "(" << face[0] << ", " << face[0] << ", " << face[0] << ")" << std::endl;
        }
    }
    /*
    std::cout <<  "Vertices: " << vertices.size() << std::endl;
    for (auto& v : vertices) {
        std::cout  << v.position << std::endl;
    }
    */

    // @ Set the vertex buffer
    if (!device) {
        throw std::runtime_error("Device not set in model");
    }
    // * Pass the vector address to the buffer
    vertexBuffer = device->newBuffer(&vertices, sizeof(Vertex) * vertices.size(), MTL::ResourceStorageModeManaged);
    if (!vertexBuffer) {
        throw std::runtime_error("Failed to create vertex buffer for model");
    } else {
        std::cout << "Vertex buffer for model created!" << std::endl;
    }
}

MTL::Buffer * Model::getVertexBuffer() {
    if (!vertexBuffer) {
        throw std::runtime_error("Failed to get vertex buffer for model");
    }
    return vertexBuffer;
}
