//
// Created by alex on 20.02.24.
//

#include "MeshIo.h"

#include "FilePath.h"
#include <map>
#include <iostream>
#include "MathConstants.h"
#include "happly.h"

namespace Bcg {

    MeshIo::MeshIo(std::string filename, const MeshIoFlags &flags) : m_filename(
            std::move(filename)), m_flags(flags) {

    }

    bool MeshIo::read(Mesh &mesh) {
        // clear mesh before reading from file
        mesh.clear();

        // extract file extension
        std::string ext = FilePath::Extension(m_filename);
        if (!FilePath::Exists(m_filename))
            return false;

        // extension determines reader
        if (ext == ".off") {
            return read_off(mesh);
        } else if (ext == ".obj") {
            return read_obj(mesh);
        } else if (ext == ".stl") {
            return read_stl(mesh);
        } else if (ext == ".ply") {
            return read_ply(mesh);
        } else if (ext == ".pmp") {
            return read_pmp(mesh);
        } else if (ext == ".xyz") {
            return read_xyz(mesh);
        } else if (ext == ".agi") {
            return read_agi(mesh);
        }

        // we didn't find a reader module
        return false;
    }

    bool MeshIo::write(Mesh &mesh) {
        // extract file extension
        std::string ext = FilePath::Extension(m_filename);

        // extension determines reader
        if (ext == ".off") {
            return write_off(mesh);
        } else if (ext == ".obj") {
            return write_obj(mesh);
        } else if (ext == ".stl") {
            return write_stl(mesh);
        } else if (ext == ".ply") {
            return write_ply(mesh);
        } else if (ext == ".pmp") {
            return write_pmp(mesh);
        } else if (ext == ".xyz") {
            return write_xyz(mesh);
        }

        // we didn't find a writer module
        return false;
    }

    bool ReadOffAscii(Mesh &mesh, FILE *in, const bool has_normals,
                      const bool has_texcoords, const bool has_colors) {
        char line[1000], *lp;
        int nc;
        unsigned int i, j, items, idx;
        unsigned int nv, nf, ne;
        double x, y, z, r, g, b;
        VertexHandle v;

        // properties
        Property<Eigen::Vector<double, 3>> normals;
        Property<Eigen::Vector<double, 2>> texcoords;
        Property<Eigen::Vector<double, 3>> colors;
        if (has_normals) {
            normals = mesh.vertices.get_or_add<Eigen::Vector<double, 3 >>("v_normal");
        }
        if (has_texcoords) {
            texcoords = mesh.vertices.get_or_add<Eigen::Vector<double, 2 >>("v_tex");
        }
        if (has_colors) {
            colors = mesh.vertices.get_or_add<Eigen::Vector<double, 3 >>("v_color");
        }

        // #Vertice, #Faces, #Edges
        items = fscanf(in, "%d %d %d\n", (int *) &nv, (int *) &nf, (int *) &ne);
        if (items == 0) {
            std::cerr << "OFF: fail to read numbers of elements\n";
        }

        mesh.vertices.reserve(nv);
        mesh.halfedges.reserve(2 * std::max(3 * nv, ne));
        mesh.edges.reserve(std::max(3 * nv, ne));
        mesh.faces.reserve(nf);

        // read vertices: pos [normal] [color] [texcoord]
        for (i = 0; i < nv && !feof(in); ++i) {
            // read line
            lp = fgets(line, 1000, in);
            lp = line;

            // position
            items = sscanf(lp, "%lf %lf %lf%n", &x, &y, &z, &nc);
            assert(items == 3);
            v = mesh.add_vertex(Eigen::Vector<double, 3>(x, y, z));
            lp += nc;

            // normal
            if (has_normals) {
                if (sscanf(lp, "%lf %lf %lf%n", &x, &y, &z, &nc) == 3) {
                    normals[v] = Eigen::Vector<double, 3>(x, y, z);
                }
                lp += nc;
            }

            // color
            if (has_colors) {
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
            if (has_texcoords) {
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
        for (i = 0; i < nf; ++i) {
            // read line
            lp = fgets(line, 1000, in);
            lp = line;

            // #vertices
            items = sscanf(lp, "%d%n", (int *) &nv, &nc);
            assert(items == 1);
            vertices.resize(nv);
            lp += nc;

            // indices
            for (j = 0; j < nv; ++j) {
                items = sscanf(lp, "%d%n", (int *) &idx, &nc);
                assert(items == 1);
                vertices[j] = VertexHandle(idx);
                lp += nc;
            }
            if (vertices.size() == nv) {
                f = mesh.add_face(vertices);
            } else {
                std::cerr << "OFF: fail to read face " << i;
            }
        }

        return true;
    }


// helper function
    template<typename T>
    void TFRead(FILE *in, const T &t) {
        size_t num_items = fread((char *) &t, 1, sizeof(t), in);
        if (num_items == 0) {
            std::cerr << "num_items should be > 0";
            assert(num_items > 0);
        }
    }

// helper function
    template<typename T>
    void TFWrite(FILE *out, const T &t) {
        size_t num_items = fwrite((char *) &t, 1, sizeof(t), out);
        if (num_items == 0) {
            std::cerr << "num_items should be > 0";
            assert(num_items > 0);
        }
    }

    bool ReadOffBinary(Mesh &mesh, FILE *in, const bool has_normals,
                       const bool has_texcoords, const bool has_colors) {
        int i, j, idx(0);
        int nv(0), nf(0), ne(0);
        Eigen::Vector<double, 3> p, n, c;
        Eigen::Vector<double, 2> t;
        VertexHandle v;

        // binary cannot (yet) read colors
        if (has_colors) {
            return false;
        }

        // properties
        Property<Eigen::Vector<double, 3>> positions = mesh.vertices.get_or_add<Eigen::Vector<double, 3 >>(
                "v_position");
        Property<Eigen::Vector<double, 3>> normals;
        Property<Eigen::Vector<double, 2>> texcoords;
        if (has_normals) {
            normals = mesh.vertices.get_or_add<Eigen::Vector<double, 3 >>("v_normal");
        }
        if (has_texcoords) {
            texcoords = mesh.vertices.get_or_add<Eigen::Vector<double, 2 >>("v_tex");
        }

        // #Vertice, #Faces, #Edges
        TFRead(in, nv);
        TFRead(in, nf);
        TFRead(in, ne);
        mesh.vertices.reserve(nv);
        mesh.halfedges.reserve(2 * std::max(3 * nv, ne));
        mesh.edges.reserve(std::max(3 * nv, ne));
        mesh.faces.reserve(nf);

        // read vertices: pos [normal] [color] [texcoord]
        for (i = 0; i < nv && !feof(in); ++i) {
            // position
            TFRead(in, p);
            v = mesh.add_vertex(p, positions);

            // normal
            if (has_normals) {
                TFRead(in, n);
                normals[v] = n;
            }

            // tex coord
            if (has_texcoords) {
                TFRead(in, t);
                texcoords[v][0] = t[0];
                texcoords[v][1] = t[1];
            }
        }

        // read faces: #N v[1] v[2] ... v[n-1]
        std::vector<VertexHandle> vertices;
        FaceHandle f;
        for (i = 0; i < nf; ++i) {
            TFRead(in, nv);
            vertices.resize(nv);
            for (j = 0; j < nv; ++j) {
                TFRead(in, idx);
                vertices[j] = VertexHandle(idx);
            }
            f = mesh.add_face(vertices);
        }

        return true;
    }

    bool MeshIo::read_off(Mesh &mesh) {
        char line[200];
        bool has_texcoords = false;
        bool has_normals = false;
        bool has_colors = false;
        bool has_hcoords = false;
        bool has_dim = false;
        bool is_binary = false;

        // open file (in ASCII mode)
        FILE *in = fopen(m_filename.c_str(), "r");
        if (!in) {
            return false;
        }

        // read header: [ST][C][N][4][n]OFF BINARY
        char *c = fgets(line, 200, in);
        assert(c != nullptr);
        c = line;
        if (c[0] == 'S' && c[1] == 'T') {
            has_texcoords = true;
            c += 2;
        }
        if (c[0] == 'C') {
            has_colors = true;
            ++c;
        }
        if (c[0] == 'N') {
            has_normals = true;
            ++c;
        }
        if (c[0] == '4') {
            has_hcoords = true;
            ++c;
        }
        if (c[0] == 'n') {
            has_dim = true;
            ++c;
        }
        if (strncmp(c, "OFF", 3) != 0) {
            fclose(in);
            return false;
        } // No OFF
        if (strncmp(c + 4, "BINARY", 6) == 0) {
            is_binary = true;
        }

        // homogeneous coords, and vertex dimension != 3 are not supported
        if (has_hcoords || has_dim) {
            fclose(in);
            return false;
        }

        // if binary: reopen file in binary mode
        if (is_binary) {
            fclose(in);
            in = fopen(m_filename.c_str(), "rb");
            c = fgets(line, 200, in);
            assert(c != nullptr);
        }

        // read as ASCII or binary
        bool ok = (is_binary ? ReadOffBinary(mesh, in, has_normals, has_texcoords, has_colors)
                             : ReadOffAscii(mesh, in, has_normals, has_texcoords, has_colors));

        if (in) {
            fclose(in);
        }
        return ok;
    }

    bool MeshIo::read_obj(Mesh &mesh) {
        char s[200];
        double x, y, z;
        std::vector<VertexHandle> vertices;
        std::vector<Eigen::Vector<double, 2>>
                all_tex_coords; //individual texture coordinates
        std::vector<int> halfedge_tex_idx; //texture coordinates sorted for halfedges
        Property<Eigen::Vector<double, 2>>
                tex_coords = mesh.halfedges.get_or_add<Eigen::Vector<double, 2 >>(
                "h_tex");
        bool with_tex_coord = false;

        // open file (in ASCII mode)
        FILE *in = fopen(m_filename.c_str(), "r");
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
                if (sscanf(s, "v %lf %lf %lf", &x, &y, &z) == 3) {
                    v = mesh.add_vertex(Eigen::Vector<double, 3>(x, y, z));
                }
            }

                // normal
            else if (strncmp(s, "vn ", 3) == 0) {
                if (sscanf(s, "vn %lf %lf %lf", &x, &y, &z) == 3) {
                    // problematic as it can be either a vertex Property when interpolated
                    // or a halfedge Property for hard edges
                }
            }

                // texture coordinate
            else if (strncmp(s, "vt ", 3) == 0) {
                auto result = sscanf(s, "vt %lf %lf", &x, &y);
                if (result == 2) {
                    all_tex_coords.emplace_back(x, y);
                }
            }

                // face
            else if (strncmp(s, "f ", 2) == 0) {
                int component(0);
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

                    // read next vertex component
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

    bool MeshIo::read_stl(Mesh &mesh) {
        char line[100], *c;
        unsigned int i, nT(0);
        Eigen::Vector<float, 3> p;
        VertexHandle v;
        std::vector<VertexHandle> vertices(3);

        CmpVec comp(std::numeric_limits<float>::min());
        std::map<Eigen::Vector<double, 3>, VertexHandle, CmpVec> vMap(comp);
        std::map<Eigen::Vector<double, 3>, VertexHandle, CmpVec>::iterator
                vMapIt;

        // open file (in ASCII mode)
        FILE *in = fopen(m_filename.c_str(), "r");
        if (!in) return false;

        // ASCII or binary STL?
        c = fgets(line, 6, in);
        assert(c != nullptr);
        const bool binary = ((strncmp(line, "SOLID", 5) != 0) && (strncmp(line, "solid", 5) != 0));

        // parse binary STL
        if (binary) {
            size_t n_items(0);
            // re-open file in binary mode
            fclose(in);
            in = fopen(m_filename.c_str(), "rb");
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
                        v = mesh.add_vertex(p.cast<double>());
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
                            v = mesh.add_vertex(p.cast<double>());
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

    bool MeshIo::read_ply(Mesh &mesh) {
        happly::PLYData plyIn(m_filename);

        std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
        std::vector<std::vector<size_t>> fInd = plyIn.getFaceIndices<size_t>();

        mesh.vertices.reserve(vPos.size());

        for (const auto &point: vPos) {
            const Eigen::Vector<double, 3> p(point.data());
            mesh.add_vertex(p);
        }
        mesh.faces.reserve(fInd.size());
        for (const auto &face: fInd) {
            mesh.add_face({face[0], face[1], face[2]});
        }

        return mesh.faces.get_size() > 0;
    }

    bool MeshIo::read_pmp(Mesh &mesh) {
        // open file (in binary mode)
        FILE *in = fopen(m_filename.c_str(), "rb");
        if (!in) {
            return false;
        }

        // how many elements?
        unsigned int nv(0), ne(0), nh(0), nf(0);
        TFRead(in, nv);
        TFRead(in, ne);
        TFRead(in, nf);
        nh = 2 * ne;

        // texture coordinates?
        bool has_htex(m_flags.use_halfedge_texcoords);
        TFRead(in, has_htex);

        // resize containers
        mesh.vertices.resize(nv);
        mesh.halfedges.resize(nh);
        mesh.edges.resize(ne);
        mesh.faces.resize(nf);

        // get properties
        auto vconn = mesh.vertices.get_or_add<Mesh::VertexConnectivity>("v_connectivity");
        auto hconn = mesh.halfedges.get_or_add<Mesh::HalfedgeConnectivity>("h_connectivity");
        auto hfconn = mesh.halfedges.get_or_add<FaceHandle>("hf_connectivity");
        auto fconn = mesh.faces.get_or_add<Mesh::FaceConnectivity>("f_connectivity");
        auto point = mesh.vertices.get_or_add<Eigen::Vector<double,
                3 >>("v_position");

        // read properties from file
        size_t nvc = fread((char *) vconn.get_ptr(), sizeof(Mesh::VertexConnectivity), nv, in);
        size_t nhc = fread((char *) hconn.get_ptr(), sizeof(Mesh::HalfedgeConnectivity), nh, in);
        size_t nhfc = fread((char *) hfconn.get_ptr(), sizeof(FaceHandle), nh, in);
        size_t nfc = fread((char *) fconn.get_ptr(), sizeof(Mesh::FaceConnectivity), nf, in);
        size_t np = fread((char *) point.get_ptr(), sizeof(Eigen::Vector<double, 3>), nv, in);
        if (nvc != nv) {
            std::cerr << "nvc != nv";
            fclose(in);
            return false;
        }
        if (nhc != nh) {
            std::cerr << "nhc != nh";
            fclose(in);
            return false;
        }
        if (nhfc != nh) {
            std::cerr << "nhfc != nh";
            fclose(in);
            return false;
        }
        if (nfc != nf) {
            std::cerr << "nfc != nf";
            fclose(in);
            return false;
        }
        if (np != nv) {
            std::cerr << "np != nv";
            fclose(in);
            return false;
        }

        // read texture coordiantes
        if (has_htex) {
            auto htex = mesh.halfedges.get_or_add<Eigen::Vector<double,
                    2 >>("h_tex");
            size_t nhtc = fread((char *) htex.get_ptr(), sizeof(Eigen::Vector<double, 2>), nh, in);
            if (nhtc != nh) {
                std::cerr << "nhtc != nh";
                fclose(in);
                return false;
            }
        }

        fclose(in);
        return true;
    }

    bool MeshIo::read_xyz(Mesh &mesh) {
        // open file (in ASCII mode)
        FILE *in = fopen(m_filename.c_str(), "r");
        if (!in) {
            return false;
        }

        // add normal Property
        auto vnormal = mesh.vertices.get_or_add<Eigen::Vector<double,
                3 >>("v_normal");

        char line[200];
        double x, y, z;
        double nx, ny, nz;
        int n;
        VertexHandle v;

        // read data
        bool has_normals = false;

        while (in && !feof(in) && fgets(line, 200, in)) {
            n = sscanf(line, "%lf %lf %lf %lf %lf %lf", &x, &y, &z, &nx, &ny, &nz);
            if (n >= 3) {
                v = mesh.add_vertex(Eigen::Vector<double, 3>(x, y, z));
                if (n >= 6) {
                    has_normals = true;
                    vnormal[v] = Eigen::Vector<double, 3>(nx, ny, nz);
                }
            }
        }
        if (!has_normals) {
            mesh.vertices.erase(vnormal);
        }

        fclose(in);
        return true;
    }

    bool MeshIo::read_agi(Mesh &mesh) {
        // open file (in ASCII mode)
        FILE *in = fopen(m_filename.c_str(), "r");
        if (!in) {
            return false;
        }

        // add normal Property
        auto normal = mesh.vertices.get_or_add<Eigen::Vector<double,
                3 >>("v_normal");
        auto color = mesh.vertices.get_or_add<Eigen::Vector<double,
                3 >>("v_color");

        char line[200];
        double x, y, z;
        double nx, ny, nz;
        double r, g, b;
        int n;
        VertexHandle v;

        // read data

        while (in && !feof(in) && fgets(line, 200, in)) {
            n = sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf", &x, &y, &z, &r, &g, &b, &nx, &ny, &nz);
            if (n >= 3) {
                v = mesh.add_vertex(Eigen::Vector<double, 3>(x, y, z));
                if (n >= 6) {
                    color[v] = Eigen::Vector<double, 3>(r / 255.0, g / 255.0, b / 255.0);
                    if (n >= 9) {
                        normal[v] = Eigen::Vector<double, 3>(nx, ny, nz);
                    }
                }
            }
        }

        fclose(in);
        return true;
    }

    bool MeshIo::write_off(Mesh &mesh) {
        if (m_flags.use_binary) {
            return write_off_binary(mesh);
        }

        FILE *out = fopen(m_filename.c_str(), "w");
        if (!out) {
            return false;
        }

        bool has_normals = false;
        bool has_texcoords = false;
        bool has_colors = false;

        auto normals = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_normal");
        auto texcoords = mesh.vertices.get<Eigen::Vector<double,
                2 >>("v_tex");
        auto colors = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_color");

        if (normals && m_flags.use_vertex_normals) {
            has_normals = true;
        }
        if (texcoords && m_flags.use_vertex_texcoords) {
            has_texcoords = true;
        }
        if (colors && m_flags.use_vertex_colors) {
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
        auto position = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_position");
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

    bool MeshIo::write_off_binary(Mesh &mesh) {
        FILE *out = fopen(m_filename.c_str(), "w");
        if (!out) {
            return false;
        }

        fprintf(out, "OFF BINARY\n");
        fclose(out);

        int nv = (int) mesh.num_vertices();
        int nf = (int) mesh.num_faces();
        int ne = 0;

        out = fopen(m_filename.c_str(), "ab");
        TFWrite(out, nv);
        TFWrite(out, nf);
        TFWrite(out, ne);
        auto positions = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_position");
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

    bool MeshIo::write_obj(Mesh &mesh) {
        FILE *out = fopen(m_filename.c_str(), "w");
        if (!out) {
            return false;
        }

        // comment
        fprintf(out, "# OBJ export from SurfaceMesh\n");

        //vertices
        auto positions = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_position");
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

    bool MeshIo::write_stl(Mesh &mesh) {
        if (!mesh.is_triangle_mesh()) {
            std::cerr << "write_stl: not a triangle mesh!\n";
            return false;
        }

        auto fnormals = mesh.faces.get<Eigen::Vector<double,
                3 >>("f_normal");
        if (!fnormals) {
            std::cerr << "write_stl: no face normals present!\n";
            return false;
        }

        std::ofstream ofs(m_filename.c_str());
        auto positions = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_position");

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

    bool MeshIo::write_ply(Mesh &mesh) {
        std::vector<std::array<double, 3>> meshVertexPositions;
        std::vector<std::vector<size_t>> meshFaceIndices;

        meshVertexPositions.reserve(mesh.num_vertices());
        meshFaceIndices.reserve(mesh.num_faces());
        auto positions = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_position");
        for (const auto v: mesh.vertices) {
            meshVertexPositions.push_back({positions[v][0], positions[v][1], positions[v][2]});
        }
        auto triangles = mesh.get_triangles();

// Create an empty object
        happly::PLYData plyOut;

// add mesh data (elements are created automatically)
        plyOut.addVertexPositions(meshVertexPositions);
        plyOut.addFaceIndices(meshFaceIndices);


// write the object to file
        plyOut.write(m_filename, happly::DataFormat::ASCII);
        return true;
    }

    bool MeshIo::write_pmp(Mesh &mesh) {
        // open file (in binary mode)
        FILE *out = fopen(m_filename.c_str(), "wb");
        if (!out) {
            return false;
        }

        // get properties
        auto vconn = mesh.vertices.get<Mesh::VertexConnectivity>("v_connectivity");
        auto hconn = mesh.halfedges.get<Mesh::HalfedgeConnectivity>("h_connectivity");
        auto hfconn = mesh.halfedges.get<FaceHandle>("hf_connectivity");
        auto fconn = mesh.faces.get<Mesh::FaceConnectivity>("f_connectivity");
        auto point = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_position");
        auto htex = mesh.halfedges.get<Eigen::Vector<double,
                2 >>("v_tex");

        // how many elements?
        unsigned int nv = (unsigned int) mesh.num_vertices();
        unsigned int ne = (unsigned int) mesh.num_edges();
        unsigned int nh = (unsigned int) mesh.num_halfedges();
        unsigned int nf = (unsigned int) mesh.num_faces();

        // write header
        TFWrite(out, nv);
        TFWrite(out, ne);
        TFWrite(out, nf);
        TFWrite(out, (bool) htex);

        // write properties to file
        fwrite((char *) vconn.get_ptr(), sizeof(Mesh::VertexConnectivity), nv, out);
        fwrite((char *) hconn.get_ptr(), sizeof(Mesh::HalfedgeConnectivity), nh, out);
        fwrite((char *) hfconn.get_ptr(), sizeof(FaceHandle), nh, out);
        fwrite((char *) fconn.get_ptr(), sizeof(Mesh::FaceConnectivity), nf, out);
        fwrite((char *) point.get_ptr(), sizeof(Eigen::Vector<double, 3>), nv, out);

        // texture coordinates
        if (htex) {
            fwrite((char *) htex.get_ptr(), sizeof(Eigen::Vector<double, 2>), nh, out);
        }

        fclose(out);
        return true;
    }

    bool MeshIo::write_xyz(Mesh &mesh) {

        std::ofstream ofs(m_filename);
        if (!ofs) {
            return false;
        }

        auto positions = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_position");
        auto vnormal = mesh.vertices.get<Eigen::Vector<double,
                3 >>("v_normal");

        for (const auto v: mesh.vertices) {
            ofs << positions[v].transpose();
            ofs << " ";
            if (vnormal) {
                ofs << vnormal[v].transpose();
            }
            ofs << std::endl;
        }

        ofs.close();

        return true;
    }

    std::vector<std::string> MeshIo::formats() {
        std::vector<std::string> names;
        names.emplace_back(".off");
        names.emplace_back(".obj");
        names.emplace_back(".stl");
        names.emplace_back(".ply");
        names.emplace_back(".pmp");
        names.emplace_back(".xyz");
        names.emplace_back(".agi");
        return names;
    }

    bool MeshIo::has_file_extension(const std::string &filename) {
        auto known_formats = formats();
        auto ext = FilePath::Extension(filename);
        return std::find(known_formats.begin(), known_formats.end(), ext) != known_formats.end();
    }
}
