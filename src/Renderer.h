//
// Created by Hayden Chalin on 5/1/25.
//

#pragma once
#include <Metal/Metal.hpp>

#include "Window.h"


class Renderer {
public:
    Renderer(Window window);
    ~Renderer();

private:
    MTL::Device *device{nullptr};
};



