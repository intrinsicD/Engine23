//
// Created by alex on 21.02.24.
//

#include "MeshReaderOff.h"
#include <iostream>

namespace Bcg {
    bool MeshReaderOff::read() {
        if (!MeshReader::read()) return false;
        if (!read_header()) return false;

        // read as ASCII or binary
        return (header.is_binary ? read_binary() : read_ascii());
    }

    bool MeshReaderOff::read_header() {
        char line[200];

        // open file (in ASCII mode)
        FILE *in = fopen(filepath.c_str(), "r");
        if (!in) {
            return false;
        }

        // read header: [ST][C][N][4][n]OFF BINARY
        char *c = fgets(line, 200, in);
        assert(c != nullptr);
        c = line;
        if (c[0] == 'S' && c[1] == 'T') {
            header.has_texcoords = true;
            c += 2;
        }
        if (c[0] == 'C') {
            header.has_colors = true;
            ++c;
        }
        if (c[0] == 'N') {
            header.has_normals = true;
            ++c;
        }
        if (c[0] == '4') {
            header.has_hcoords = true;
            ++c;
        }
        if (c[0] == 'n') {
            header.has_dim = true;
            ++c;
        }
        if (strncmp(c, "OFF", 3) != 0) {
            fclose(in);
            return false;
        } // No OFF
        if (strncmp(c + 4, "BINARY", 6) == 0) {
            header.is_binary = true;
        }

        // homogeneous coords, and vertex dimension != 3 are not supported
        if (header.has_hcoords || header.has_dim) {
            fclose(in);
            return false;
        }

        if (in) {
            fclose(in);
        }

        return true;
    }

    bool MeshReaderOff::read_binary() {
        // if binary: reopen file in binary mode
        FILE *in = fopen(filepath.c_str(), "rb");
        if (!in) {
            return false;
        }
        char line[200];
        char *c = fgets(line, 200, in);
        assert(c != nullptr);

        // properties
        auto positions = mesh.vertices.get_or_add<Eigen::Vector<double, 3 >>("v_position");
        Property<Eigen::Vector<double, 3>> normals;
        Property<Eigen::Vector<double, 3>> colors;
        Property<Eigen::Vector<double, 2>> texcoords;

        if (header.has_normals) {
            normals = mesh.vertices.get_or_add<Eigen::Vector<double, 3 >>("v_normal");
        }
        if (header.has_colors) {
            colors = mesh.vertices.get_or_add<Eigen::Vector<double, 3 >>("v_color");
        }
        if (header.has_texcoords) {
            texcoords = mesh.vertices.get_or_add<Eigen::Vector<double, 2 >>("v_texcoord");
        }

        // #Vertice, #Faces, #Edges
        int nv(0), nf(0), ne(0);
        TFRead(in, nv);
        TFRead(in, nf);
        TFRead(in, ne);
        mesh.vertices.reserve(nv);
        mesh.halfedges.reserve(2 * std::max(3 * nv, ne));
        mesh.edges.reserve(std::max(3 * nv, ne));
        mesh.faces.reserve(nf);

        // read vertices: pos [normal] [color] [texcoord]
        VertexHandle v;
        Eigen::Vector<double, 3> point;
        Eigen::Vector<double, 2> tex;
        for (int i = 0; i < nv && !feof(in); ++i) {
            // position
            TFRead(in, point);
            v = mesh.add_vertex(point, positions);

            // normal
            if (header.has_normals) {
                TFRead(in, normals[v]);
            }

            if (header.has_colors) {
                TFRead(in, colors[v]);
            }

            // tex coord
            if (header.has_texcoords) {
                TFRead(in, tex);
                texcoords[v][0] = tex[0];
                texcoords[v][1] = tex[1];
            }
        }

        // read faces: #N v[1] v[2] ... v[n-1]
        std::vector<VertexHandle> vertices;
        FaceHandle f;
        int idx(0);
        for (int i = 0; i < nf; ++i) {
            TFRead(in, nv);
            vertices.resize(nv);
            for (int j = 0; j < nv; ++j) {
                TFRead(in, idx);
                vertices[j].idx = idx;
            }
            f = mesh.add_face(vertices);
        }

        return true;
    }

    bool MeshReaderOff::read_ascii() {
        FILE *in = fopen(filepath.c_str(), "r");
        if (!in) {
            return false;
        }

        char line[1000], *lp;
        int nc;
        unsigned int items, idx;
        unsigned int nv, nf, ne;
        double x, y, z, r, g, b;
        VertexHandle v;

        // properties
        auto positions = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_position");
        Property<Eigen::Vector<double, 3>> normals;
        Property<Eigen::Vector<double, 2>> texcoords;
        Property<Eigen::Vector<double, 3>> colors;
        if (header.has_normals) {
            normals = mesh.vertices.get_or_add<Eigen::Vector<double, 3 >>("v_normal");
        }
        if (header.has_colors) {
            colors = mesh.vertices.get_or_add<Eigen::Vector<double, 3 >>("v_color");
        }
        if (header.has_texcoords) {
            texcoords = mesh.vertices.get_or_add<Eigen::Vector<double, 2 >>("v_tex");
        }

        char *c = fgets(line, 200, in);
        assert(c != nullptr);

        // #Vertice, #Faces, #Edges
        items = fscanf(in, "%d %d %d\n", (int *) &nv, (int *) &nf, (int *) &ne);
        if (items == 0) {
            std::cerr << "OFF: fail to read numbers of elements\n";
            return false;
        }

        mesh.vertices.reserve(nv);
        mesh.halfedges.reserve(2 * std::max(3 * nv, ne));
        mesh.edges.reserve(std::max(3 * nv, ne));
        mesh.faces.reserve(nf);

        // read vertices: pos [normal] [color] [texcoord]
        for (int i = 0; i < nv && !feof(in); ++i) {
            // read line
            lp = fgets(line, 1000, in);
            lp = line;

            // position
            items = sscanf(lp, "%lf %lf %lf%n", &x, &y, &z, &nc);
            assert(items == 3);
            v = mesh.add_vertex(Eigen::Vector<double, 3>(x, y, z), positions);
            lp += nc;

            // normal
            if (header.has_normals) {
                if (sscanf(lp, "%lf %lf %lf%n", &x, &y, &z, &nc) == 3) {
                    normals[v] = Eigen::Vector<double, 3>(x, y, z);
                }
                lp += nc;
            }

            // color
            if (header.has_colors) {
                if (sscanf(lp, "%lf %lf %lf%n", &r, &g, &b, &nc) == 3) {
                    if (r > 1.0 || g > 1.0 || b > 1.0) {
                        r /= 255.0;
                        g /= 255.0;
                        b /= 255.0;
                    }
                    colors[v] = Eigen::Vector<double, 3>(r, g, b);
                }
                lp += nc;
            }

            // tex coord
            if (header.has_texcoords) {
                items = sscanf(lp, "%lf %lf%n", &x, &y, &nc);
                assert(items == 2);
                texcoords[v][0] = x;
                texcoords[v][1] = y;
                lp += nc;
            }
        }

        // read faces: #N v[1] v[2] ... v[n-1]
        std::vector<VertexHandle> vertices;
        FaceHandle f;
        for (int i = 0; i < nf; ++i) {
            // read line
            lp = fgets(line, 1000, in);
            lp = line;

            // #vertices
            items = sscanf(lp, "%d%n", (int *) &nv, &nc);
            assert(items == 1);
            vertices.resize(nv);
            lp += nc;

            // indices
            for (int j = 0; j < nv; ++j) {
                items = sscanf(lp, "%d%n", (int *) &idx, &nc);
                assert(items == 1);
                vertices[j] = VertexHandle(idx);
                lp += nc;
            }
            if (vertices.size() == nv) {
                f = mesh.add_face(vertices);
            } else {
                std::cerr << "OFF: fail to read face " << i;
                return false;
            }
        }

        return true;
    }
}