//
// Created by Hayden Chalin on 9/6/25.
//

/*
 *   The controller class will controll the camera movement by communicating
 *   with glfw's callback functions
 *
 */


#pragma once

#include <GLFW/glfw3.h>

class Camera;
class Window;


class Controller {
    public:
    Controller();
    Controller(Camera * camera, Window * window);
    ~Controller();

    void pollEvents();
    // Key map
    bool keyMap[GLFW_KEY_LAST + 1] = {false};
    void processKey(int key, int scancode, int action, int mods);
    // Smooth per-frame update based on key state and dt
    void update(float dt);

    private:
    Camera *camera;
    Window * window;
};

// * Define glfw free functions

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);


