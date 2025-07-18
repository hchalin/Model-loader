//
// Created by Hayden Chalin on 7/14/25.
//


#pragma once

#include <vector>
#include "Model.h"

#include "Renderer.h"
#include "Window.h"
class Scene {
public:
    Scene();
    ~Scene();
    void start();

    Model * loadModels();

    private:
    //std::vector<Model> models;
    Model* model;
    Window* window;
    Renderer* renderer;



};


