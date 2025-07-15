//
// Created by Hayden Chalin on 7/14/25.
//

#include "Model.h"
//#include <jsoncpp>
#include <json/json.h>

Model::Model(std::string &fileNamme): gltfFilename(fileNamme),
                                       vertexBuffer(nullptr)
{
   if (fileNamme == "") throw std::invalid_argument("File name missing");
   loadModel();
}

void Model::loadModel() {
    Json::Value root;
}

