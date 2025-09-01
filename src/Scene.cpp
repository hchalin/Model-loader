//
// Created by Hayden Chalin on 7/14/25.
//

#include "Scene.h"

#include <iostream>
#include <ostream>

Scene::Scene()
    : camera(Eigen::Vector3f{0, 0, -1}, Eigen::Vector3f{0, 0, 0}),
      controller(camera, nullptr) {
}


void Scene::start() {
    // @ Try and create the window (will also create the device)
    try {
        window = std::make_unique<Window>();
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
         renderer = std::make_unique<Renderer>(*window, model.get(), &camera);
         renderer->render();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

std::unique_ptr<Model> Scene::loadModel() {
    // Define which obj file you want to load here
    std::string fileName = "CC2.obj";
    // std::string fileName = "scene_test.obj";
    if (!device) {
        throw std::runtime_error("No device in loadModels()");
    }


    return make_unique<Model>(device, fileName);
}
