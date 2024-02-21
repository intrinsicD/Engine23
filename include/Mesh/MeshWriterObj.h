//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHWRITEROBJ_H
#define ENGINE23_MESHWRITEROBJ_H

#include "MeshWriter.h"

namespace Bcg{
    class MeshWriterObj : public MeshWriter{
    public:
        using MeshWriter::MeshWriter;

        bool write() override;
    };
}


#endif //ENGINE23_MESHWRITEROBJ_H
