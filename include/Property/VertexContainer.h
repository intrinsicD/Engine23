//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_VERTEXCONTAINER_H
#define ENGINE23_VERTEXCONTAINER_H

#include "PropertyContainer.h"

namespace Bcg{
    class VertexContainer;

    struct VertexHandle : public HandleBase<VertexHandle> {
        using HandleBase::HandleBase;
    };

    using VertexIterator = PropertyIterator<VertexContainer, VertexHandle>;

    class VertexContainer : public PropertyContainer {
    public:
        Property<bool> deleted;

        VertexContainer();

        ~VertexContainer() override = default;

        void clear() override;

        VertexIterator begin();

        VertexIterator end();

        VertexIterator begin() const;

        VertexIterator end() const;
    };
}

#endif //ENGINE23_VERTEXCONTAINER_H
