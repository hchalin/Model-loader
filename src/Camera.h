//
// Created by Hayden Chalin on 5/29/25.
//

#pragma once
#include <eigen/Eigen/Dense>
#include <iostream>

using namespace Eigen;

class Camera {
    public:
    Camera(const Vector3f& position, const Vector3f& target);

    Matrix4f &getViewMatrix();

    private:
    Vector3f camPos = Vector3f(0,0,3.0);
    Vector3f camTarget = Vector3f(0,0,0); // origin
    Vector3f camDirection;
    Vector3f camLookAt;
    Vector3f camUp;
    Vector3f camRight;

    Matrix4f viewMatrix;     // Note: World Space -> Camera Space
};

