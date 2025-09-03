//
// Created by Hayden Chalin on 7/14/25.
//

#include "Scene.h"

#include <iostream>
#include <ostream>

Scene::Scene():
    device(MTL::CreateSystemDefaultDevice())        // Create the GPU device
{
    // @ Try and create the window (will also create the device)
    try {
        window = new Window(this->device);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    // @ Create camera w/ windows aspect ratio
    camera = new Camera(Vector3f(0, 0, 5.0f), Vector3f(0, 0, 0), window->getAspectRatio());

    // @ After window creation, load models
    model = loadModel();            // This returns a pointer to a model created


    // @ After window creation, and model loading, start rendering
    try {
         renderer = new Renderer(this->device, *window, model);
         renderer->render(camera->getViewMatrix(), camera->getProjectionMatrix(), model->getModelMatrix());
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
Scene::~Scene() {
    delete window;
    window = nullptr;
    delete renderer;
    renderer = nullptr;
    delete model;
    model = nullptr;
}


Model * Scene::loadModel() {
    // Define which obj file you want to load here
    device = window->getMTLLayer()->device();
    std::string fileName = "CC2.obj";
    // std::string fileName = "scene_test.obj";
    if (!device) {
        throw std::runtime_error("No device in loadModels()");
    }

    model = new Model(device, fileName);

    return model;
}
