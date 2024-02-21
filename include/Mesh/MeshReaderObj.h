//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHREADEROBJ_H
#define ENGINE23_MESHREADEROBJ_H

#include "MeshReader.h"

namespace Bcg{
    class MeshReaderObj : public MeshReader{
    public:
        using MeshReader::MeshReader;

        bool read() override;
    };
}

#endif //ENGINE23_MESHREADEROBJ_H
