//
// Created by Hayden Chalin on 7/14/25.
//

#include "Scene.h"

#include <iostream>
#include <ostream>

Scene::Scene() {
}
Scene::~Scene() {

}

void Scene::start() {
    try {
        window = new Window();
         renderer = new Renderer(*window, loadModel());
         renderer->render();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

Model * Scene::loadModel() {
    MTL::Device * device = window->getMTLLayer()->device();
    std::string fileName = "Cube.obj";
    if (!device) {
        throw std::runtime_error("No device in loadModels()");
    }
    model = new Model(device, fileName);
    return model;
}
