//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHREADEROFF_H
#define ENGINE23_MESHREADEROFF_H

#include "MeshReader.h"

namespace Bcg{
    class MeshReaderOff : public MeshReader{
    public:
        using MeshReader::MeshReader;

        struct Header{
            bool has_texcoords = false;
            bool has_normals = false;
            bool has_colors = false;
            bool has_hcoords = false;
            bool has_dim = false;
            bool is_binary = false;
        }header;

        bool read() override;

        bool read_header();

        bool read_binary();

        bool read_ascii();
    };
}

#endif //ENGINE23_MESHREADEROFF_H
