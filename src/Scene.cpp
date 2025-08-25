//
// Created by Hayden Chalin on 7/14/25.
//

#include "Scene.h"

#include <iostream>
#include <ostream>

Scene::Scene() {
}
Scene::~Scene() {
    delete window;
    window = nullptr;
    delete renderer;
    renderer = nullptr;
    delete model;
    model = nullptr;
}

void Scene::start() {
    // @ Try and create the window (will also create the device)
    try {
        window = new Window();
        device = window->getMTLLayer()->device();       // Set device after window creation
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    // @ After window creation, load models
    if (window) {
        model = loadModel();
    } else {
        throw std::runtime_error(std::string("Failed to load model in file: ")+ __FILE__ + " " + std::to_string(__LINE__));
    }


    // @ After window creation, and model loading, start rendering
    try {
         renderer = new Renderer(*window, model);
         renderer->render();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

Model * Scene::loadModel() {
    // Define which obj file you want to load here
    device = window->getMTLLayer()->device();
    std::string fileName = "lowpoly-house.obj";
    if (!device) {
        throw std::runtime_error("No device in loadModels()");
    }

    model = new Model(device, fileName);

    return model;
}
