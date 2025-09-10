//
// Created by Hayden Chalin on 7/14/25.
//

#include "Scene.h"

#include <iostream>
#include <ostream>

using namespace Eigen;

Scene::Scene():
    device(MTL::CreateSystemDefaultDevice())        // Create the GPU device
{
    // @ Try and create the window
    try {
        window = new Window(this->device);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    // @ Create camera w/ windows aspect ratio
    camera = new Camera(Vector3f(0, 0, 15.0f), Vector3f(0, 0, 0), window->getAspectRatio());
    window->setCamera(camera);

    // @ After window creation, load models
    model = loadModel();            // This returns a pointer to a model created


    // @ Before you render, after creating a window and camera, create a controller
    controller = new Controller(camera, window);
    window->setController(controller);


    // @ After window creation, and model loading, start rendering
    try {
        renderer = new Renderer(this->device, *window, model, camera);
        window->setRenderer(renderer);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }


    // Run the scene
    run();
}
Scene::~Scene() {
    delete window;
    window = nullptr;
    delete renderer;
    renderer = nullptr;
    delete model;
    model = nullptr;
}

void Scene::run() {


    while (!glfwWindowShouldClose(window->getGLFWWindow())) {
        // ^ Timeing
        computeDelta();
        controller->pollEvents();
        controller->update(window->getDeltaTime());
        renderer->drawFrame();
    }
}

float Scene::computeDelta() {
    const auto currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastTime;
    lastTime = currentFrame;
    totalTime += deltaTime;
    window->setDeltaTime(deltaTime);
    return deltaTime;
}

Model * Scene::loadModel() {
    // Define which obj file you want to load here
    std::string fileName = "CC3.obj";
    if (!device) {
        throw std::runtime_error("No device in loadModels()");
    }

    model = new Model(device, fileName);

    return model;
}