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


class Model {
public:
    //Model() = default;
    Model( std::string& fileNamme);
    ~Model() = default;
    // Methods
    void loadModel(MTL::Device * device);

    private:
    std::string& glbFilename;
    MTL::Device * device;
    // Buffers
    MTL::Buffer* vertexBuffer;


};



#endif //MODEL_H
