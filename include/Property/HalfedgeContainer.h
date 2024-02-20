//
// Created by alex on 20.02.24.
//

#ifndef ENGINE23_HALFEDGECONTAINER_H
#define ENGINE23_HALFEDGECONTAINER_H

#include "PropertyContainer.h"

namespace Bcg{
    class HalfedgeContainer;

    struct HalfedgeHandle : public HandleBase<HalfedgeHandle> {
        using HandleBase::HandleBase;
    };

    using HalfedgeIterator = PropertyIterator<HalfedgeContainer, HalfedgeHandle>;

    class HalfedgeContainer : public PropertyContainer {
    public:
        Property<bool> deleted;

        HalfedgeContainer();

        ~HalfedgeContainer() override = default;

        void clear() override;

        HalfedgeIterator begin();

        HalfedgeIterator end();

        HalfedgeIterator begin() const;

        HalfedgeIterator end() const;
    };
}

#endif //ENGINE23_HALFEDGECONTAINER_H
