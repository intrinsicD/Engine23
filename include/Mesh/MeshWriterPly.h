//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHWRITERPLY_H
#define ENGINE23_MESHWRITERPLY_H

#include "MeshWriter.h"

namespace Bcg {
    class MeshWriterPly : public MeshWriter {
    public:
        using MeshWriter::MeshWriter;

        bool write() override;
    };
}
#endif //ENGINE23_MESHWRITERPLY_H
