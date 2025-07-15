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
    try {
        Window window;
        Renderer renderer = Renderer(window);
        renderer.render();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void Scene::loadModels() {

}
