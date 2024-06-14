//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_PROPERTY_H
#define ENGINE23_PROPERTY_H

#include "PropertyVector.h"
#include "HandleBase.h"
#include <memory>
#include <utility>

namespace Bcg {
    template<typename T>
    class Property {
    public:
        using Storage_t = typename PropertyVector<T>::Storage_t;
        using Reference_t = typename PropertyVector<T>::Reference_t;
        using ConstReference_t = typename PropertyVector<T>::ConstReference_t;
        using Iterator_t = typename PropertyVector<T>::Iterator_t;
        using ConstIterator_t = typename PropertyVector<T>::ConstIterator_t;

        Property() = default;

        explicit Property(std::shared_ptr<PropertyVector<T>> sptr);

        operator bool() const;

        Reference_t operator[](size_t i);

        ConstReference_t operator[](size_t i) const;

        template<typename Derived>
        Reference_t operator[](const HandleBase<Derived> &i) {
            return operator[](size_t(i));
        }

        template<typename Derived>
        ConstReference_t operator[](const HandleBase<Derived> &i) const {
            return operator[](size_t(i));
        }

        Reference_t back();

        ConstReference_t back() const;

        [[nodiscard]] size_t get_dims() const;

        [[nodiscard]] size_t get_dims_bytes() const;

        [[nodiscard]] size_t get_size() const;

        [[nodiscard]] size_t get_size_bytes() const;

        [[nodiscard]] size_t get_capacity() const;

        T *get_ptr();

        const T *get_ptr() const;

        [[nodiscard]] const void *get_void_ptr() const;

        void swap(size_t i0, size_t i1);

        [[nodiscard]] bool is_empty() const;

        [[nodiscard]] bool is_dirty() const;

        void clear();

        void free_unused_memory();

        void resize(size_t n);

        void reserve(size_t n);

        void reset(const T &t = T());

        void push_back();

        void push_back(const T &value);

        const T &default_value() const;

        const T &zero_value() const;

        [[nodiscard]] const std::string &get_name() const;

        void set_dirty();

        void set_clean();

        Iterator_t begin();

        Iterator_t end();

        ConstIterator_t begin() const;

        ConstIterator_t end() const;

        [[nodiscard]] std::string info() const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] std::string to_string(size_t idx) const;

        void set_column_names(std::string column_names);

        Storage_t &storage();

        const Storage_t &storage() const;

        std::shared_ptr<const PropertyVector<T>> get_const_sptr() const;

    protected:

        std::shared_ptr<PropertyVector<T>> m_sptr;
    };

    template<typename T>
    Property<T>::Property(std::shared_ptr<PropertyVector<T>> sptr) : m_sptr(std::move(sptr)) {}

    template<typename T>
    Property<T>::operator bool() const {
        return m_sptr != nullptr;
    }

    template<typename T>
    typename Property<T>::Reference_t Property<T>::operator[](size_t i) { return m_sptr->operator[](i); }

    template<typename T>
    typename Property<T>::ConstReference_t Property<T>::operator[](size_t i) const {
        return get_const_sptr()->operator[](i);
    }

    template<typename T>
    typename Property<T>::Reference_t Property<T>::back() {
        return m_sptr->back();
    }

    template<typename T>
    typename Property<T>::ConstReference_t Property<T>::back() const {
        return get_const_sptr()->back();
    }

    template<typename T>
    size_t Property<T>::get_dims() const { return get_const_sptr()->get_dims(); }

    template<typename T>
    size_t Property<T>::get_dims_bytes() const { return get_const_sptr()->get_dims_bytes(); }

    template<typename T>
    size_t Property<T>::get_size() const { return get_const_sptr()->get_size(); }

    template<typename T>
    size_t Property<T>::get_size_bytes() const { return get_const_sptr()->get_size_bytes(); }

    template<typename T>
    size_t Property<T>::get_capacity() const { return get_const_sptr()->capacity(); }

    template<typename T>
    T *Property<T>::get_ptr() { return m_sptr->get_ptr(); }

    template<typename T>
    const T *Property<T>::get_ptr() const { return get_const_sptr()->get_ptr(); }

    template<typename T>
    const void *Property<T>::get_void_ptr() const { return static_cast<const void *>(get_ptr()); }

    template<typename T>
    void Property<T>::swap(size_t i0, size_t i1) { m_sptr->swap(i0, i1); }

    template<typename T>
    bool Property<T>::is_empty() const { return get_const_sptr()->is_empty(); }

    template<typename T>
    bool Property<T>::is_dirty() const { return get_const_sptr()->is_dirty(); }

    template<typename T>
    void Property<T>::clear() { m_sptr->clear(); }

    template<typename T>
    void Property<T>::free_unused_memory() { m_sptr->free_unused_memory(); }

    template<typename T>
    void Property<T>::resize(size_t n) { m_sptr->resize(n); }

    template<typename T>
    void Property<T>::reserve(size_t n) { m_sptr->reserve(n); }

    template<typename T>
    void Property<T>::reset(const T &t) { m_sptr->reset(t); }

    template<typename T>
    void Property<T>::push_back() { m_sptr->push_back(); }

    template<typename T>
    void Property<T>::push_back(const T &value) { m_sptr->push_back(value); }

    template<typename T>
    const T &Property<T>::default_value() const { return m_sptr->default_value(); }

    template<typename T>
    const T &Property<T>::zero_value() const {
        return default_value() - default_value();
    }

    template<typename T>
    const std::string &Property<T>::get_name() const { return get_const_sptr()->get_name(); }

    template<typename T>
    void Property<T>::set_dirty() {
        m_sptr->set_dirty();
    }

    template<typename T>
    void Property<T>::set_clean() {
        m_sptr->set_clean();
    }

    template<typename T>
    typename Property<T>::Iterator_t Property<T>::begin() {
        return m_sptr->begin();
    }

    template<typename T>
    typename Property<T>::Iterator_t Property<T>::end() {
        return m_sptr->end();
    }

    template<typename T>
    typename Property<T>::ConstIterator_t Property<T>::begin() const {
        return m_sptr->begin();
    }

    template<typename T>
    typename Property<T>::ConstIterator_t Property<T>::end() const {
        return m_sptr->end();
    }

    template<typename T>
    std::shared_ptr<const PropertyVector<T>> Property<T>::get_const_sptr() const {
        return std::const_pointer_cast<const PropertyVector<T>>(m_sptr);
    }

    template<typename T>
    [[nodiscard]] std::string Property<T>::info() const{
        return get_const_sptr()->info();
    }

    template<typename T>
    [[nodiscard]] std::string Property<T>::to_string() const{
        return get_const_sptr()->to_string();
    }

    template<typename T>
    [[nodiscard]] std::string Property<T>::to_string(size_t idx) const{
        return get_const_sptr()->to_string(idx);
    }

    template<typename T>
    void Property<T>::set_column_names(std::string column_names) {
        m_sptr->set_column_names(column_names);
    }
    template<typename T>
    typename Property<T>::Storage_t &Property<T>::storage(){
        return m_sptr->storage();
    }
    template<typename T>
    const typename Property<T>::Storage_t &Property<T>::storage() const{
        return get_const_sptr()->storage();
    }

    //------------------------------------------------------------------------------------------------------------------


    //------------------------------------------------------------------------------------------------------------------
}
#endif //ENGINE23_PROPERTY_H
