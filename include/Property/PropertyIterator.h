//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_PROPERTYITERATOR_H
#define ENGINE23_PROPERTYITERATOR_H

#include <iterator>

template<typename Container, typename HandleType>
struct PropertyIterator {
    using iterator_category = std::bidirectional_iterator_tag;

    explicit PropertyIterator(HandleType handle, const Container *container = nullptr);

    HandleType operator*() const;

    bool operator==(PropertyIterator rhs) const;

    bool operator!=(PropertyIterator rhs) const;

    bool operator<(PropertyIterator rhs) const;

    bool operator>(PropertyIterator rhs) const;

    PropertyIterator &operator++();

    PropertyIterator &operator--();

private:
    HandleType m_handle;
    const Container *m_container;
};

template<typename Container, typename HandleType>
PropertyIterator<Container, HandleType>::PropertyIterator(HandleType handle, const Container *container) : m_handle(
        handle),
                                                                                                           m_container(
                                                                                                                   container) {
    if (m_container) {
        if (m_container->deleted) {
            auto size = m_container->get_size();
            while (m_handle < size && m_container->deleted[size_t(m_handle)]) {
                ++m_handle;
            }
        }
    }
}

template<typename Container, typename HandleType>
HandleType PropertyIterator<Container, HandleType>::operator*() const {
    return m_handle;
}

template<typename Container, typename HandleType>
bool PropertyIterator<Container, HandleType>::operator==(PropertyIterator rhs) const {
    return m_handle == rhs.m_handle;
}

template<typename Container, typename HandleType>
bool PropertyIterator<Container, HandleType>::operator!=(PropertyIterator rhs) const {
    return m_handle != rhs.m_handle;
}

template<typename Container, typename HandleType>
bool PropertyIterator<Container, HandleType>::operator<(PropertyIterator rhs) const {
    return m_handle < rhs.m_handle;
}

template<typename Container, typename HandleType>
bool PropertyIterator<Container, HandleType>::operator>(PropertyIterator rhs) const {
    return m_handle > rhs.m_handle;
}

template<typename Container, typename HandleType>
PropertyIterator<Container, HandleType> &PropertyIterator<Container, HandleType>::operator++() {
    ++m_handle;
    if (m_container->deleted) {
        auto size = m_container->get_size();
        while (m_handle < size && m_container->deleted[size_t(m_handle)]) {
            ++m_handle;
        }
    }
    return *this;
}

template<typename Container, typename HandleType>
PropertyIterator<Container, HandleType> &PropertyIterator<Container, HandleType>::operator--() {
    --m_handle;
    if (m_container->deleted) {
        auto size = m_container->get_size();
        while (m_handle > 0 && m_handle < size && m_container->deleted[size_t(m_handle)]) {
            --m_handle;
        }
    }
    return *this;
}

#endif //ENGINE23_PROPERTYITERATOR_H
