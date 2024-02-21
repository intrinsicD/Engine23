//
// Created by alex on 21.02.24.
//

#include "MeshReaderStl.h"
#include <map>

namespace Bcg{
    class CmpVec {
    public:
        explicit CmpVec(double eps = std::numeric_limits<double>::epsilon()) : eps_(eps) {}

        bool operator()(const Eigen::Vector<double, 3> &v0, const Eigen::Vector<double, 3> &v1) const {
            if (std::abs(v0[0] - v1[0]) <= eps_) {
                if (std::abs(v0[1] - v1[1]) <= eps_) {
                    return (v0[2] < v1[2] - eps_);
                } else {
                    return (v0[1] < v1[1] - eps_);
                }
            } else {
                return (v0[0] < v1[0] - eps_);
            }
        }

    private:
        double eps_;
    };

    bool MeshReaderStl::read() {
        if (!MeshReader::read()) return false;

        char line[100], *c;
        unsigned int i, nT(0);
        Eigen::Vector<float, 3> p;
        VertexHandle v;
        std::vector<VertexHandle> vertices(3);

        CmpVec comp(std::numeric_limits<float>::min());
        std::map<Eigen::Vector<double, 3>, VertexHandle, CmpVec> vMap(comp);
        std::map<Eigen::Vector<double, 3>, VertexHandle, CmpVec>::iterator vMapIt;

        auto positions = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_position");

        // open file (in ASCII mode)
        FILE *in = fopen(filepath.c_str(), "r");
        if (!in){
            return false;
        }

        // ASCII or binary STL?
        c = fgets(line, 6, in);
        assert(c != nullptr);
        const bool binary = ((strncmp(line, "SOLID", 5) != 0) && (strncmp(line, "solid", 5) != 0));

        // parse binary STL
        if (binary) {
            size_t n_items(0);
            // re-open file in binary mode
            fclose(in);
            in = fopen(filepath.c_str(), "rb");
            if (!in) {
                return false;
            }

            // skip dummy header
            n_items = fread(line, 1, 80, in);
            assert(n_items > 0);

            // read number of triangles
            TFRead(in, nT);

            // read triangles
            while (nT) {
                // skip triangle normal
                n_items = fread(line, 1, 12, in);
                assert(n_items > 0);
                // triangle's vertices
                for (i = 0; i < 3; ++i) {
                    TFRead(in, p);

                    // has vector been referenced before?
                    if ((vMapIt = vMap.find(p.cast<double>())) == vMap.end()) {
                        // No : add vertex and remember idx/vector mapping
                        v = mesh.add_vertex(Eigen::Vector<double, 3>(p[0], p[1], p[2]), positions);
                        vertices[i] = v;
                        vMap[p.cast<double>()] = v;
                    } else {
                        // Yes : get index from map
                        vertices[i] = vMapIt->second;
                    }
                }

                // add face only if it is not degenerated
                FaceHandle f;
                if ((vertices[0] != vertices[1]) &&
                    (vertices[0] != vertices[2]) &&
                    (vertices[1] != vertices[2])) {
                    f = mesh.add_face(vertices);
                }

                n_items = fread(line, 1, 2, in);
                assert(n_items > 0);
                --nT;
            }
        }

            // parse ASCII STL
        else {
            // parse line by line
            while (in && !feof(in) && fgets(line, 100, in)) {
                // skip white-space
                for (c = line; isspace(*c) && *c != '\0'; ++c) {}

                // face begins
                if ((strncmp(c, "outer", 5) == 0) || (strncmp(c, "OUTER", 5) == 0)) {
                    // read three vertices
                    for (i = 0; i < 3; ++i) {
                        // read line
                        c = fgets(line, 100, in);
                        assert(c != nullptr);

                        // skip white-space
                        for (c = line; isspace(*c) && *c != '\0'; ++c) {}

                        // read x, y, z
                        sscanf(c + 6, "%f %f %f", &p[0], &p[1], &p[2]);

                        // has vector been referenced before?
                        if ((vMapIt = vMap.find(p.cast<double>())) == vMap.end()) {
                            // No : add vertex and remember idx/vector mapping
                            v = mesh.add_vertex(Eigen::Vector<double, 3>(p[0], p[1], p[2]), positions);
                            vertices[i] = v;
                            vMap[p.cast<double>()] = v;
                        } else {
                            // Yes : get index from map
                            vertices[i] = vMapIt->second;
                        }
                    }

                    // add face only if it is not degenerated
                    FaceHandle f;
                    if ((vertices[0] != vertices[1]) &&
                        (vertices[0] != vertices[2]) &&
                        (vertices[1] != vertices[2]))
                        f = mesh.add_face(vertices);
                }
            }
        }

        fclose(in);
        return true;
    }
}