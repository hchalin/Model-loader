//
// Created by Hayden Chalin on 5/29/25.
//

#include "Camera.h"

Camera::Camera(const Vector3f& position, const Vector3f& lookAt):
    up(Vector3f(0.0, 1.0, 0.0)),
    right(1.0, 0.0,0.0),
    forward(0.0, 0.0, -1.0),
    position(position)
    {}

