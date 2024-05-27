//
// Created by alex on 27.05.24.
//

#ifndef ENGINE23_CLICKPOINT_H
#define ENGINE23_CLICKPOINT_H

#include "Eigen/Core"

namespace Bcg {
    struct Point {
        bool is_background = false;
        Eigen::Vector<float, 3> object{};
        Eigen::Vector<float, 3> world{};
        Eigen::Vector<float, 3> view{};
        Eigen::Vector<float, 2> win_coords{};
        Eigen::Vector<float, 2> norm_dev_coords{};
    };

    Eigen::Vector<float, 2>
    WindowCoordsToNormalizedDeviceCoordinates(const Eigen::Vector<float, 2> &win_coords, int width, int height);

    Eigen::Vector<float, 2>
    NormalizedDeviceCoordinatesToWindowCoords(const Eigen::Vector<float, 2> &ndc_coords, int width, int height);

    Eigen::Vector<float, 3> NormalizedDeviceCoordinatesToViewCoords(const Eigen::Vector<float, 2> &nd_coords,
                                                                    float depth,
                                                                    const Eigen::Matrix<float, 4, 4> &proj);

    Eigen::Vector<float, 2> ViewCoordsToNormalizedDeviceCoordinates(const Eigen::Vector<float, 3> &view_coords,
                                                                    const Eigen::Matrix<float, 4, 4> &proj);

    Eigen::Vector<float, 3> ViewCoordsToWorldCoords(const Eigen::Vector<float, 3> &view_coords,
                                              const Eigen::Matrix<float, 4, 4> &view);

    Eigen::Vector<float, 3> WorldCoordsToViewCoords(const Eigen::Vector<float, 3> &world_coords,
                                                    const Eigen::Matrix<float, 4, 4> &view);

    Eigen::Vector<float, 3> WorldCoordsToObjectCoords(const Eigen::Vector<float, 3> &world_coords,
                                              const Eigen::Matrix<float, 4, 4> &model);

    Eigen::Vector<float, 3> ObjectCoordsToWorldCoords(const Eigen::Vector<float, 3> &obj_coords,
                                                      const Eigen::Matrix<float, 4, 4> &model);

}

#endif //ENGINE23_CLICKPOINT_H
