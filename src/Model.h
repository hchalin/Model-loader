//
// Created by Hayden Chalin on 7/14/25.
//

#ifndef MODEL_H
#define MODEL_H

#include <Metal/MTLBuffer.hpp>


class Model {
public:
    //Model() = default;
    Model(std::string& fileNamme);
    ~Model() = default;

    private:
    std::string& gltfFilename;

    // Buffers
    MTL::Buffer* vertexBuffer;


    // Methods
    void loadModel();
};



#endif //MODEL_H
