//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_CAMERAARCBALLPARAMETERS_H
#define ENGINE23_CAMERAARCBALLPARAMETERS_H

#include "Camera.h"

namespace Bcg{
    struct ArcBallCameraParameters {
        Camera &camera;
        bool last_point_ok = false;
        glm::vec3 target = glm::vec3(0.0f);
        glm::vec2 last_point_2d = glm::vec2(0.0f);
        glm::vec3 last_point_3d = glm::vec3(0.0f);
    };
}

#endif //ENGINE23_CAMERAARCBALLPARAMETERS_H
