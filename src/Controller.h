//
// Created by Hayden Chalin on 6/26/25.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Window.h"
#include "Camera.h"

class Controller {
public:
    Controller(Camera &camera, Window *window);

    ~Controller();
    void handleKey(int key, int scancode, int action, int mods);
};



#endif //CONTROLLER_H
