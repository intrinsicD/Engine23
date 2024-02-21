//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHREADERPMP_H
#define ENGINE23_MESHREADERPMP_H
#include "MeshReader.h"

namespace Bcg{
    class MeshReaderPmp : public MeshReader{
    public:
        using MeshReader::MeshReader;

        bool read() override;
    };
}
#endif //ENGINE23_MESHREADERPMP_H
