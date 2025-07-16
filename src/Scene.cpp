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
    std::cout << "Scene start" << std::endl;
// ? This was just copied and pasted from the old main, make these class member/methods
    try {
        window = new Window();
         renderer = new Renderer(*window);
         loadModels();      // Load models before rendering
         renderer->render();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }


}

void Scene::loadModels() {
    std::cout << "Loading models" << std::endl;
    MTL::Device * device = window->getMTLLayer()->device();
    std::string fileName = "Suzanne.glb";
    if (!device) {
        throw std::runtime_error("No device in loadModels()");
    }
    model = new Model(fileName);
    model->loadModel(device);
}
