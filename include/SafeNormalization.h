//
// Created by alex on 08.03.24.
//

#ifndef ENGINE23_SAFENORMALIZATION_H
#define ENGINE23_SAFENORMALIZATION_H

namespace Bcg{
    template<typename T, typename S>
    T SafeNormalize(const T &element, S length, S epsilon = 1e-8){
        return element / (length + epsilon);
    }
}

#endif //ENGINE23_SAFENORMALIZATION_H
