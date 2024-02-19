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

    class VertexIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;

        explicit VertexIterator(VertexHandle handle, const VertexContainer *container = nullptr);

        VertexHandle operator*() const;

        bool operator==(VertexIterator rhs) const;

        bool operator!=(VertexIterator rhs) const;

        bool operator<(VertexIterator rhs) const;

        bool operator>(VertexIterator rhs) const;

        VertexIterator &operator++();

        VertexIterator &operator--();

    private:
        VertexHandle m_handle;
        const VertexContainer *m_container;
    };

    class VertexContainer : public PropertyContainer {
    public:
        Property<bool> deleted;

        VertexContainer();

        ~VertexContainer() override = default;

        void clear() override;


        auto &&indices() { return PropertyContainer::indices<VertexIterator, VertexContainer>(); }

    };
}

#endif //ENGINE23_VERTEXCONTAINER_H
