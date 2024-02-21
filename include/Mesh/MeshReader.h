//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHREADER_H
#define ENGINE23_MESHREADER_H

#include <string>
#include "Mesh.h"

namespace Bcg {
    class MeshReader {
    public:
        explicit MeshReader(std::string filepath, Mesh &mesh);

        virtual ~MeshReader() = default;

        virtual bool read();

    protected:
        template<typename T>
        void TFRead(FILE *in, const T &t) {
            size_t num_items = fread((char *) &t, 1, sizeof(t), in);
            if (num_items == 0) {
                assert(num_items > 0);
            }
        }

        std::string filepath;
        Mesh &mesh;
    };
}

#endif //ENGINE23_MESHREADER_H
