//
// Created by Hayden Chalin on 5/29/25.
//

#pragma once
#include <eigen/Eigen/Dense>

using namespace Eigen;

class Camera {
    public:
    Camera(const Vector3f& position, const Vector3f& lookAt);

    private:
    Vector3f up;
    Vector3f right;
    Vector3f forward;
    Vector3f position;      // Do i need this?

};

