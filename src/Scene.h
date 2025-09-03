//
// Created by Hayden Chalin on 7/14/25.
//


#pragma once

#include <vector>
#include "Model.h"

#include "Renderer.h"
#include "Window.h"
#include <fstream>

class Scene {
public:
    Scene();

    ~Scene();


    Model *loadModel();


private:

    Camera *camera {nullptr};
    Model *model {nullptr};
    Window *window {nullptr};
    Renderer *renderer {nullptr};
    MTL::Device *device {nullptr};
};


