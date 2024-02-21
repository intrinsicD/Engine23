//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHREADERPLY_H
#define ENGINE23_MESHREADERPLY_H

#include "MeshReader.h"

namespace Bcg{
    class MeshReaderPly : public MeshReader{
    public:
        using MeshReader::MeshReader;

        bool read() override;
    };
}
#endif //ENGINE23_MESHREADERPLY_H
