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

    void start();

    Model *loadModel();


private:

    Camera camera;
    Model *model;
    Window *window;
    Renderer *renderer;
    MTL::Device *device;
};


