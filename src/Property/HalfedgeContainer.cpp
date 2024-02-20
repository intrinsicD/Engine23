//
// Created by alex on 20.02.24.
//

#include "HalfedgeContainer.h"

namespace Bcg {
    HalfedgeContainer::HalfedgeContainer() : PropertyContainer("Halfedges"), deleted(get_or_add<bool>("h_deleted", false)) {
        assert(deleted);
    }

    void HalfedgeContainer::clear(){
        deleted = get_or_add<bool>("h_deleted", false);
        assert(deleted);
    }

    HalfedgeIterator HalfedgeContainer::begin() {
        return HalfedgeIterator(HalfedgeHandle(0), this);
    }

    HalfedgeIterator HalfedgeContainer::end() {
        return HalfedgeIterator(HalfedgeHandle(get_size()), this);
    }

    HalfedgeIterator HalfedgeContainer::begin() const {
        return HalfedgeIterator(HalfedgeHandle(0), this);
    }

    HalfedgeIterator HalfedgeContainer::end() const {
        return HalfedgeIterator(HalfedgeHandle(get_size()), this);
    }
}