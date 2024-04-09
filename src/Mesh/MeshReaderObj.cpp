//
// Created by alex on 21.02.24.
//

#include "MeshReaderObj.h"

namespace Bcg {
    bool MeshReaderObj::read() {
        if (!MeshReader::read()) return false;

        char s[200];
        float x, y, z;
        std::vector<VertexHandle> vertices;
        std::vector<Eigen::Vector<double, 2>> all_tex_coords; //individual texture coordinates
        std::vector<int> halfedge_tex_idx; //texture coordinates sorted for halfedges
        auto tex_coords = mesh.halfedges.get_or_add<Eigen::Vector<double, 2>>("h_tex");
        auto positions = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_position");
        bool with_tex_coord = false;

        // open file (in ASCII mode)
        FILE *in = fopen(filepath.c_str(), "r");
        if (!in) {
            return false;
        }

        // clear line once
        memset(&s, 0, 200);

        // parse line by line (currently only supports vertex positions & faces
        VertexHandle v;
        while (in && !feof(in) && fgets(s, 200, in)) {
            // comment
            if (s[0] == '#' || isspace(s[0])) {
                continue;
            }

                // vertex
            else if (strncmp(s, "v ", 2) == 0) {
                if (sscanf(s, "v %f %f %f", &x, &y, &z) == 3) {
                    v = mesh.add_vertex(Eigen::Vector<double, 3>(x, y, z), positions);
                }
            }

                // normal
            else if (strncmp(s, "vn ", 3) == 0) {
                if (sscanf(s, "vn %f %f %f", &x, &y, &z) == 3) {
                    // problematic as it can be either a vertex Property when interpolated
                    // or a halfedge Property for hard edges
                }
            }

                // texture coordinate
            else if (strncmp(s, "vt ", 3) == 0) {
                auto result = sscanf(s, "vt %f %f", &x, &y);
                if (result == 2) {
                    all_tex_coords.emplace_back(x, y);
                }
            }

                // face
            else if (strncmp(s, "f ", 2) == 0) {
                int component(0), nv(0);
                bool end_of_vertex(false);
                char *p0, *p1(s + 1);

                vertices.clear();
                halfedge_tex_idx.clear();

                // skip white-spaces
                while (*p1 == ' ') {
                    ++p1;
                }

                while (p1) {
                    p0 = p1;

                    // overwrite next separator

                    // skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
                    while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' && *p1 != ' ' && *p1 != '\0') {
                        ++p1;
                    }

                    // detect end of vertex
                    if (*p1 != '/') {
                        end_of_vertex = true;
                    }

                    // replace separator by '\0'
                    if (*p1 != '\0') {
                        *p1 = '\0';
                        p1++; // point to next token
                    }

                    // detect end of line and break
                    if (*p1 == '\0' || *p1 == '\n') {
                        p1 = nullptr;
                    }

                    // read next vertex Component
                    if (*p0 != '\0') {
                        switch (component) {
                            case 0: // vertex
                            {
                                vertices.emplace_back(atoi(p0) - 1);
                                break;
                            }
                            case 1: // texture coord
                            {
                                int idx = atoi(p0) - 1;
                                halfedge_tex_idx.push_back(idx);
                                with_tex_coord = true;
                                break;
                            }
                            case 2: // normal
                                break;
                        }
                    }

                    ++component;

                    if (end_of_vertex) {
                        component = 0;
                        nv++;
                        end_of_vertex = false;
                    }
                }

                FaceHandle f = mesh.add_face(vertices);

                // add texture coordinates
                if (with_tex_coord && f.is_valid()) {
                    unsigned v_idx = 0;
                    for (const auto h: mesh.get_halfedges(f)) {
                        tex_coords[h] = all_tex_coords[halfedge_tex_idx[v_idx]];
                        ++v_idx;
                    }
                }
            }
            // clear line
            memset(&s, 0, 200);
        }

        // if there are no textures, delete texture Property!
        if (!with_tex_coord) {
            mesh.halfedges.erase(tex_coords);
        }

        fclose(in);
        return true;
    }
}