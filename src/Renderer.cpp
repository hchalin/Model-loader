//
// Created by Hayden Chalin on 5/1/25.
//

#include "Renderer.h"

#include <iostream>
#include <ostream>

#include "Window.h"

Renderer::Renderer(Window window) {

    if (!device) {
        throw std::runtime_error("Failed to create MTL::Device");
    } else {
        std::cout << "Created MTL::Device" << std::endl;
    }
}
Renderer::~Renderer() {

}

