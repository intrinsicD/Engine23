//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHWRITEROFF_H
#define ENGINE23_MESHWRITEROFF_H

#include "MeshWriter.h"

namespace Bcg {
    class MeshWriterOff : public MeshWriter {
    public:
        using MeshWriter::MeshWriter;

        bool write() override;

        bool write_binary();

        bool write_ascii();

        bool use_binary = false;
        bool use_vertex_normals = false;
        bool use_vertex_texcoords = false;
        bool use_vertex_colors = false;
    };
}

#endif //ENGINE23_MESHWRITEROFF_H
