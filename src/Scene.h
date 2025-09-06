//
// Created by Hayden Chalin on 7/14/25.
//


#pragma once

#include <vector>
#include "Model.h"

#include "Renderer.h"
// #include "Window.h"
// #include "Camera.h"
#include "Controller.h"

class Scene {
public:
    Scene();

    ~Scene();


    Model *loadModel();


private:

    Controller *controller{nullptr};
    Camera *camera {nullptr};
    Model *model {nullptr};
    Window *window {nullptr};
    Renderer *renderer {nullptr};
    MTL::Device *device {nullptr};
};


