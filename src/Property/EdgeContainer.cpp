//
// Created by alex on 20.02.24.
//

#include "EdgeContainer.h"
#include <assert.h>

namespace Bcg {
    EdgeContainer::EdgeContainer() : PropertyContainer("Edges"), deleted(get_or_add<bool>("e_deleted", false)) {
        assert(deleted);
    }

    void EdgeContainer::clear(){
        deleted = get_or_add<bool>("e_deleted", false);
        assert(deleted);
    }

    EdgeIterator EdgeContainer::begin() {
        return EdgeIterator(EdgeHandle(0), this);
    }

    EdgeIterator EdgeContainer::end() {
        return EdgeIterator(EdgeHandle(get_size()), this);
    }

    EdgeIterator EdgeContainer::begin() const {
        return EdgeIterator(EdgeHandle(0), this);
    }

    EdgeIterator EdgeContainer::end() const {
        return EdgeIterator(EdgeHandle(get_size()), this);
    }
}