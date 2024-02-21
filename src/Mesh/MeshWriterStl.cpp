//
// Created by alex on 21.02.24.
//

#include "MeshWriterStl.h"
#include <iostream>
#include <fstream>

namespace Bcg {
    bool MeshWriterStl::write() {
        if (!mesh.is_triangle_mesh()) {
            std::cerr << "write_stl: not a triangle mesh!\n";
            return false;
        }

        auto fnormals = mesh.faces.get<Eigen::Vector<double,
                3 >>("f_normal");
        if (!fnormals) {
            std::cerr << "write_stl: requires face normals!\n";
            return false;
        }

        std::ofstream ofs(filepath.c_str());
        auto positions = mesh.vertices.get<Eigen::Vector<double, 3 >>("v_position");

        ofs << "solid stl" << std::endl;
        ofs.precision(10);
        ofs.setf(std::ios::fixed);
        Eigen::Vector<double, 3> n, p;
        for (const auto f: mesh.faces) {
            n = fnormals[f];
            ofs << "  facet normal ";
            ofs << n[0] << " " << n[1] << " " << n[2] << std::endl;
            ofs << "    outer loop" << std::endl;
            for (const auto v: mesh.get_vertices(f)) {
                p = positions[v];
                ofs << "      vertex ";
                ofs << p[0] << " " << p[1] << " " << p[2] << std::endl;
            }
            ofs << "    endloop" << std::endl;
            ofs << "  endfacet" << std::endl;
        }
        ofs << "endsolid" << std::endl;
        ofs.close();
        return true;
    }
}