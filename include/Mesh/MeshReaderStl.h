//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHREADERSTL_H
#define ENGINE23_MESHREADERSTL_H

#include "MeshReader.h"

namespace Bcg{
    class MeshReaderStl : public MeshReader{
    public:
        using MeshReader::MeshReader;

        bool read() override;
    };
}

#endif //ENGINE23_MESHREADERSTL_H
