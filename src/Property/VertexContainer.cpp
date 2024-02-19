//
// Created by alex on 19.02.24.
//

#include "VertexContainer.h"

namespace Bcg {
    VertexIterator::VertexIterator(VertexHandle handle, const VertexContainer *container) : m_handle(handle),
                                                                                            m_container(container) {
        if (m_container) {
            if (m_container->deleted) {
                auto size = m_container->get_size();
                while (m_handle < size && m_container->deleted[m_handle]) {
                    ++m_handle;
                }
            }
        }
    }

    VertexHandle VertexIterator::operator*() const {
        return m_handle;
    }

    bool VertexIterator::operator==(VertexIterator rhs) const {
        return m_handle == rhs.m_handle;
    }

    bool VertexIterator::operator!=(VertexIterator rhs) const {
        return m_handle != rhs.m_handle;
    }

    bool VertexIterator::operator<(VertexIterator rhs) const {
        return m_handle < rhs.m_handle;
    }

    bool VertexIterator::operator>(VertexIterator rhs) const {
        return m_handle > rhs.m_handle;
    }

    VertexIterator &VertexIterator::operator++() {
        ++m_handle;
        if (m_container->deleted) {
            auto size = m_container->get_size();
            while (m_handle < size && m_container->deleted[m_handle]) {
                ++m_handle;
            }
        }
        return *this;
    }

    VertexIterator &VertexIterator::operator--() {
        --m_handle;
        if (m_container->deleted) {
            auto size = m_container->get_size();
            while (m_handle < size && m_container->deleted[m_handle]) {
                --m_handle;
            }
        }
        return *this;
    }

    VertexContainer::VertexContainer() : PropertyContainer("Vertices"), deleted(get_or_add<bool>("v_deleted", false)) {
        assert(deleted);
    }

    void VertexContainer::clear(){
        deleted = get_or_add<bool>("v_deleted", false);
        assert(deleted);
    }

    IndicesIterator<VertexIterator, VertexContainer> VertexContainer::indices() { return IndicesIterator<VertexIterator, VertexContainer>(*this); }

    VertexIterator VertexContainer::begin() const {
        return VertexIterator(0, this);
    }

    VertexIterator VertexContainer::end() const {
        return VertexIterator(get_size(), this);
    }
}