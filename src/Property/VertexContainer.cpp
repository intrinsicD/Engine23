//
// Created by alex on 19.02.24.
//

#include "VertexContainer.h"

namespace Bcg {
    VertexContainer::VertexContainer() : PropertyContainer("Vertices"), deleted(get_or_add<bool>("v_deleted", false)) {
        assert(deleted);
    }

    void VertexContainer::clear(){
        deleted = get_or_add<bool>("v_deleted", false);
        assert(deleted);
    }

    VertexIterator VertexContainer::begin() {
        return VertexIterator(VertexHandle(0), this);
    }

    VertexIterator VertexContainer::end() {
        return VertexIterator(VertexHandle(get_size()), this);
    }

    VertexIterator VertexContainer::begin() const {
        return VertexIterator(VertexHandle(0), this);
    }

    VertexIterator VertexContainer::end() const {
        return VertexIterator(VertexHandle(get_size()), this);
    }
}