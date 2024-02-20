//
// Created by alex on 20.02.24.
//

#ifndef ENGINE23_FACECONTAINER_H
#define ENGINE23_FACECONTAINER_H

#include "PropertyContainer.h"

namespace Bcg{
    class FaceContainer;

    struct FaceHandle : public HandleBase<FaceHandle> {
        using HandleBase::HandleBase;
    };

    using FaceIterator = PropertyIterator<FaceContainer, FaceHandle>;

    class FaceContainer : public PropertyContainer {
    public:
        Property<bool> deleted;

        FaceContainer();

        ~FaceContainer() override = default;

        void clear() override;

        FaceIterator begin();

        FaceIterator end();

        FaceIterator begin() const;

        FaceIterator end() const;
    };
}
#endif //ENGINE23_FACECONTAINER_H
