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
    ~Scene() = default;
    void start();

    std::unique_ptr<Model> loadModel();

    private:
    //std::vector<Model> models;
    MTL::Device * device;
    std::unique_ptr<Model> model{nullptr};
    std::unique_ptr<Renderer> renderer{nullptr};
    std::unique_ptr<Window> window{nullptr};

    // Camera w/ controller
    Camera camera;
    Controller controller;



};


