//
// Created by alex on 21.02.24.
//

#include "MeshWriterPmp.h"

namespace Bcg{
    bool MeshWriterPmp::write() {
        // open file (in binary mode)
        FILE *out = fopen(filepath.c_str(), "wb");
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
}