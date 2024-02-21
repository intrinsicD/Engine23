//
// Created by alex on 21.02.24.
//

#include "MeshWriterObj.h"

namespace Bcg{
    bool MeshWriterObj::write() {
        FILE *out = fopen(filepath.c_str(), "w");
        if (!out) {
            return false;
        }

        // comment
        fprintf(out, "# OBJ export from Bcg\n");

        //vertices
        auto positions = mesh.vertices.get<Eigen::Vector<double,3 >>("v_position");
        for (const auto v: mesh.vertices) {
            fprintf(out, "v %.10f %.10f %.10f\n", positions[v][0], positions[v][1], positions[v][2]);
        }

        //normals
        auto normals = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_normal");
        if (normals) {
            for (const auto v: mesh.vertices) {
                fprintf(out, "vn %.10f %.10f %.10f\n", normals[v][0], normals[v][1], normals[v][2]);
            }
        }

        // optional texture coordinates
        // do we have them?

        auto texCoord = mesh.halfedges.get<Eigen::Vector<double,
                2 >>("tex");
        //if so then add
        if (texCoord) {
            for (const auto h: mesh.halfedges) {
                fprintf(out, "vt %.10f %.10f \n", texCoord[h][0], texCoord[h][1]);
            }
        }

        //faces
        for (const auto f: mesh.faces) {
            fprintf(out, "f");
            auto fvit = mesh.get_vertices(f);
            auto fvend = fvit;
            auto fhit = mesh.get_halfedges(f);
            do {
                if (texCoord) {
                    // write vertex index, texCoord index and normal index
                    fprintf(out, " %zu/%zu/%zu", (*fvit).idx + 1, (*fhit).idx + 1,
                            (*fvit).idx + 1);
                    ++fhit;
                } else {
                    // write vertex index and normal index
                    fprintf(out, " %zu//%zu", (*fvit).idx + 1, (*fvit).idx + 1);
                }
            } while (++fvit != fvend);
            fprintf(out, "\n");
        }

        fclose(out);
        return true;
    }
}