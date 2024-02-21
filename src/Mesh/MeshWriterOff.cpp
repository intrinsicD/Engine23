//
// Created by alex on 21.02.24.
//

#include "MeshWriterOff.h"

namespace Bcg{
    bool MeshWriterOff::write() {
        if(use_binary){
            return write_binary();
        }else{
            return write_ascii();
        }
    }

    bool MeshWriterOff::write_binary(){
        FILE *out = fopen(filepath.c_str(), "w");
        if (!out) {
            return false;
        }

        fprintf(out, "OFF BINARY\n");
        fclose(out);

        int nv = (int) mesh.num_vertices();
        int nf = (int) mesh.num_faces();
        int ne = 0;

        out = fopen(filepath.c_str(), "ab");
        TFWrite(out, nv);
        TFWrite(out, nf);
        TFWrite(out, ne);
        auto positions = mesh.vertices.get<Eigen::Vector<double,3 >>("v_position");

        for (const auto v: mesh.vertices) {
            TFWrite(out, positions[v]);
        }

        for (const auto f: mesh.faces) {
            TFWrite(out, (int) mesh.get_valence(f));
            for (const auto fv: mesh.get_vertices(f)) {
                TFWrite(out, (int) fv.idx);
            }
        }
        fclose(out);
        return true;
    }

    bool MeshWriterOff::write_ascii(){
        FILE *out = fopen(filepath.c_str(), "w");
        if (!out) {
            return false;
        }

        bool has_normals = false;
        bool has_texcoords = false;
        bool has_colors = false;

        auto normals = mesh.vertices.get<Eigen::Vector<double,3 >>("v_normal");
        auto colors = mesh.vertices.get<Eigen::Vector<double,3 >>("v_color");
        auto texcoords = mesh.vertices.get<Eigen::Vector<double,2 >>("v_tex");

        if (normals && use_vertex_normals) {
            has_normals = true;
        }
        if (texcoords && use_vertex_texcoords) {
            has_texcoords = true;
        }
        if (colors && use_vertex_colors) {
            has_colors = true;
        }

        // header
        if (has_texcoords) {
            fprintf(out, "ST");
        }
        if (has_colors) {
            fprintf(out, "C");
        }
        if (has_normals) {
            fprintf(out, "N");
        }
        fprintf(out, "OFF\n%zu %zu 0\n", mesh.num_vertices(), mesh.num_faces());

        // vertices, and optionally normals and texture coordinates
        auto position = mesh.vertices.get<Eigen::Vector<double,3 >>("v_position");
        for (const auto v: mesh.vertices) {
            fprintf(out, "%.10f %.10f %.10f", position[v][0], position[v][1], position[v][2]);

            if (has_normals) {
                fprintf(out, " %.10f %.10f %.10f", normals[v][0], normals[v][1], normals[v][2]);
            }

            if (has_colors) {
                fprintf(out, " %.10f %.10f %.10f", colors[v][0], colors[v][1], colors[v][2]);
            }

            if (has_texcoords) {
                fprintf(out, " %.10f %.10f", texcoords[v][0], texcoords[v][1]);
            }

            fprintf(out, "\n");
        }

        // faces
        for (const auto f: mesh.faces) {
            int nv = (int) mesh.get_valence(f);
            fprintf(out, "%d", nv);
            for (const auto fv: mesh.get_vertices(f)) {
                fprintf(out, " %zu", fv.idx);
            }
            fprintf(out, "\n");
        }

        fclose(out);
        return true;
    }
}