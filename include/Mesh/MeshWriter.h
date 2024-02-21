//
// Created by alex on 21.02.24.
//

#ifndef ENGINE23_MESHWRITER_H
#define ENGINE23_MESHWRITER_H

#include <string>
#include "Mesh.h"

namespace Bcg {
    class MeshWriter {
    public:
        explicit MeshWriter(std::string filepath, Mesh &mesh);

        virtual bool write() = 0;

    protected:
        template<typename T>
        void TFWrite(FILE *out, const T &t) {
            size_t num_items = fwrite((char *) &t, 1, sizeof(t), out);
            if (num_items == 0) {
                assert(num_items > 0);
            }
        }

        std::string filepath;
        Mesh &mesh;
    };
}
#endif //ENGINE23_MESHWRITER_H
