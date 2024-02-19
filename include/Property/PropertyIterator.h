//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_PROPERTYITERATOR_H
#define ENGINE23_PROPERTYITERATOR_H

#include <iterator>

template<typename Container, typename HandleType = size_t>
struct PropertyIterator {

    using iterator_category = std::bidirectional_iterator_tag;

    explicit PropertyIterator(HandleType handle, const Container *container = nullptr) : m_handle(handle),
                                                                                         m_container(container) {
        if (m_container) {
            if (m_container->deleted) {
                auto size = m_container->get_size();
                while (m_handle < size && m_container->deleted[m_handle.idx]) {
                    ++m_handle;
                }
            }
        }
    }

    HandleType operator*() const {
        return m_handle;
    }

    bool operator==(PropertyIterator rhs) const {
        return m_handle == rhs.handle;
    }

    bool operator!=(PropertyIterator rhs) const {
        return m_handle != rhs.handle;
    }

    bool operator<(PropertyIterator rhs) const {
        return m_handle < rhs.handle;
    }

    bool operator>(PropertyIterator rhs) const {
        return m_handle > rhs.handle;
    }

    PropertyIterator &operator++() {
        ++m_handle;
        if(m_container->deleted){
            auto size = m_container->get_size();
            while (m_handle < size && m_container->deleted[m_handle.idx]) {
                ++m_handle;
            }
        }
        return *this;
    }

    PropertyIterator &operator--() {
        --m_handle;
        if(m_container->deleted){
            auto size = m_container->get_size();
            while (m_handle > 0 && m_handle < size && m_container->deleted[m_handle.idx]) {
                --m_handle;
            }
        }
        return *this;
    }

private:
    HandleType m_handle;
    const Container *m_container;
};

template<typename HandleType, class Container>
class IndicesIterator{
public:
    explicit IndicesIterator(Container &container) : m_container(container) {}

    PropertyIterator<HandleType, Container> begin() {
        return PropertyIterator<HandleType, Container>(HandleType(0), &m_container);
    }

    PropertyIterator<HandleType, Container> end() {
        return PropertyIterator<HandleType, Container>(HandleType(m_container.get_size()),  &m_container);
    }

    PropertyIterator<HandleType, Container> begin() const {
        return PropertyIterator<HandleType, Container>(HandleType(0),  &m_container);
    }

    PropertyIterator<HandleType, Container> end() const {
        return PropertyIterator<HandleType, Container>(HalfedgeHandle(HandleType()),  &m_container);
    }
private:
    Container &m_container;
};

#endif //ENGINE23_PROPERTYITERATOR_H
