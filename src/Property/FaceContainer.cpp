//
// Created by alex on 20.02.24.
//
#include "FaceContainer.h"
#include <assert.h>

namespace Bcg {
    FaceContainer::FaceContainer() : PropertyContainer("Faces"), deleted(get_or_add<bool>("f_deleted", false)) {
        assert(deleted);
    }

    void FaceContainer::clear() {
        deleted = get_or_add<bool>("f_deleted", false);
        assert(deleted);
    }

    FaceIterator FaceContainer::begin() {
        return FaceIterator(FaceHandle(0), this);
    }

    FaceIterator FaceContainer::end() {
        return FaceIterator(FaceHandle(get_size()), this);
    }

    FaceIterator FaceContainer::begin() const {
        return FaceIterator(FaceHandle(0), this);
    }

    FaceIterator FaceContainer::end() const {
        return FaceIterator(FaceHandle(get_size()), this);
    }
}