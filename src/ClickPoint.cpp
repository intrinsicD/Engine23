//
// Created by alex on 27.05.24.
//

#include "ClickPoint.h"
#include "Eigen/Geometry"

namespace Bcg {
    Eigen::Vector<float, 2>
    WindowCoordsToNormalizedDeviceCoordinates(const Eigen::Vector<float, 2> &win_coords, int width, int height) {
        return {2.0f * win_coords[0] / width - 1.0f, 1.0f - 2.0f * win_coords[1] / height};
    }

    Eigen::Vector<float, 2>
    NormalizedDeviceCoordinatesToWindowCoords(const Eigen::Vector<float, 2> &ndc_coords, int width, int height) {
        return {(ndc_coords[0] + 1.0f) * width / 2.0f, (1.0f - ndc_coords[1]) * height / 2.0f};
    }

    Eigen::Vector<float, 3> NormalizedDeviceCoordinatesToViewCoords(const Eigen::Vector<float, 2> &nd_coords,
                                                                    float depth,
                                                                    const Eigen::Matrix<float, 4, 4> &proj) {
        auto p = (proj.inverse() * Eigen::Vector<float, 4>(nd_coords[0], nd_coords[1], depth * 2.0f - 1.0f, 1.0f));
        return p.head<3>() / p[3];
    }

    Eigen::Vector<float, 2> ViewCoordsToNormalizedDeviceCoordinates(const Eigen::Vector<float, 3> &view_coords,
                                                                    const Eigen::Matrix<float, 4, 4> &proj) {
        Eigen::Vector4f clip_coords = proj * view_coords.homogeneous();
        return {clip_coords[0] / clip_coords[3], clip_coords[1] / clip_coords[3]};

    }

    Eigen::Vector<float, 3> ViewCoordsToWorldCoords(const Eigen::Vector<float, 3> &view_coords,
                                                    const Eigen::Matrix<float, 4, 4> &view){
        return (view.inverse() * view_coords.homogeneous()).head<3>();
    }

    Eigen::Vector<float, 3> WorldCoordsToViewCoords(const Eigen::Vector<float, 3> &world_coords,
                                                    const Eigen::Matrix<float, 4, 4> &view){
        return (view * world_coords.homogeneous()).head<3>();
    }

    Eigen::Vector<float, 3> WorldCoordsToObjectCoords(const Eigen::Vector<float, 3> &world_coords,
                                                      const Eigen::Matrix<float, 4, 4> &model){
        return (model.inverse() * world_coords.homogeneous()).head<3>();
    }

    Eigen::Vector<float, 3> ObjectCoordsToWorldCoords(const Eigen::Vector<float, 3> &obj_coords,
                                                      const Eigen::Matrix<float, 4, 4> &model){
        return (model * obj_coords.homogeneous()).head<3>();
    }
}