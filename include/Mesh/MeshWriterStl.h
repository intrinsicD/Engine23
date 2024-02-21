//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHWRITERSTL_H
#define ENGINE23_MESHWRITERSTL_H

#include "MeshWriter.h"

namespace Bcg{
    class MeshWriterStl : public MeshWriter{
    public:
        using MeshWriter::MeshWriter;

        bool write() override;
    };
}
#endif //ENGINE23_MESHWRITERSTL_H
