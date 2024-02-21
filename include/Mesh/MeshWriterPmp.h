//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHWRITERPMP_H
#define ENGINE23_MESHWRITERPMP_H

#include "MeshWriter.h"

namespace Bcg {
    class MeshWriterPmp : public MeshWriter {
    public:
        using MeshWriter::MeshWriter;

        bool write() override;
    };
}
#endif //ENGINE23_MESHWRITERPMP_H
