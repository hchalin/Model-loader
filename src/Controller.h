//
// Created by Hayden Chalin on 6/23/25.
//

#pragma once
#include "Camera.h"
#include "Window.h"


class Controller {
public:
    // ! Make this a derived camera class
    Controller(Camera &cam, Window *winow); // Controller takes a camera by ref
    ~Controller();
    void handleEvents();

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



