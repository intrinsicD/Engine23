//
// Created by alex on 20.02.24.
//

#ifndef ENGINE23_MESHIO_H
#define ENGINE23_MESHIO_H

#include <string>
#include "Mesh.h"

namespace Bcg {
    struct MeshIoFlags {
        MeshIoFlags() = default;

        bool use_binary = false;             //!< read / write binary format
        bool use_vertex_normals = false;     //!< read / write vertex normals
        bool use_vertex_colors = false;      //!< read / write vertex colors
        bool use_vertex_texcoords = false;   //!< read / write vertex texcoords
        bool use_face_normals = false;       //!< read / write face normals
        bool use_face_colors = false;        //!< read / write face colors
        bool use_halfedge_texcoords = false; //!< read / write halfedge texcoords
    };

    struct MeshIo {
        MeshIo(std::string filename, const MeshIoFlags &flags);

        bool read(Mesh &mesh);

        bool write(Mesh &mesh);

        static std::vector<std::string> formats();

        bool has_file_extension(const std::string &filename);

    private:

        bool read_off(Mesh &mesh);

        bool read_obj(Mesh &mesh);

        bool read_stl(Mesh &mesh);

        bool read_ply(Mesh &mesh);

        bool read_pmp(Mesh &mesh);

        bool write_off(Mesh &mesh);

        bool write_off_binary(Mesh &mesh);

        bool write_obj(Mesh &mesh);

        bool write_stl(Mesh &mesh);

        bool write_ply(Mesh &mesh);

        bool write_pmp(Mesh &mesh);

    private:
        std::string m_filename;
        MeshIoFlags m_flags;
    };

}
#endif //ENGINE23_MESHIO_H
