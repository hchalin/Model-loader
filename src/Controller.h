//
// Created by Hayden Chalin on 6/23/25.
//

#pragma once
#include "Camera.h"
#include "Window.h"


class Controller {
public:
    // ! Make this a derived camera class
    Controller(Camera &cam, Window *window); // Controller takes a camera by ref
    ~Controller();
    void handleInput(int key, int action, int mods);
    void handleScroll(double xoffset, double yoffset);

private:
    Camera &camera;
    Window *window;

    // Camera movement w/ shift
    void camUp();
    void camdown();
    void camRight();
    void camLeft();

    // In/Out
    void camIn(); // * Scroll down w/ cmd
    void camOut(); // * Scroll up w/ cmd

    // Rotation
    void camRotateDown(); // * Scroll up
    void camRotateUp(); // * Scroll down
    void camRotateRight(); // * Scroll left
    void camRotateLeft(); // * Scroll right


};



void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
