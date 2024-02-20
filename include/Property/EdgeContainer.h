//
// Created by alex on 20.02.24.
//

#ifndef ENGINE23_EDGECONTAINER_H
#define ENGINE23_EDGECONTAINER_H

#include "PropertyContainer.h"

namespace Bcg{
    class EdgeContainer;

    struct EdgeHandle : public HandleBase<EdgeHandle> {
        using HandleBase::HandleBase;
    };

    using EdgeIterator = PropertyIterator<EdgeContainer, EdgeHandle>;

    class EdgeContainer : public PropertyContainer {
    public:
        Property<bool> deleted;

        EdgeContainer();

        ~EdgeContainer() override = default;

        void clear() override;

        EdgeIterator begin();

        EdgeIterator end();

        EdgeIterator begin() const;

        EdgeIterator end() const;
    };
}

#endif //ENGINE23_EDGECONTAINER_H
