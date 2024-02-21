//
// Created by alex on 21.02.24.
//

#include "MeshReaderPmp.h"
#include <iostream>

namespace Bcg {
    bool MeshReaderPmp::read() {
        if (!MeshReader::read()) return false;

        // open file (in binary mode)
        FILE *in = fopen(filepath.c_str(), "rb");
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
        bool has_htex(false);
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
        auto point = mesh.vertices.get_or_add<Eigen::Vector<double, 3 >>("v_position");

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
            auto htex = mesh.halfedges.get_or_add<Eigen::Vector<double, 2>>("h_tex");
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
}