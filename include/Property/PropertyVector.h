//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_PROPERTYVECTOR_H
#define ENGINE23_PROPERTYVECTOR_H

#include "AbstractProperty.h"
#include "PropertyUtils.h"

namespace Bcg {
    template<typename T>
    class PropertyVector : public AbstractProperty {
    public:
        using Storage_t = std::vector<T>;
        using Iterator_t = typename Storage_t::iterator;
        using ConstIterator_t = typename Storage_t::const_iterator;
        using Reference_t = typename Storage_t::reference;
        using ConstReference_t = typename Storage_t::const_reference;

        explicit PropertyVector(std::string name, T t = T());

        ~PropertyVector() override = default;

        Reference_t operator[](size_t i);

        ConstReference_t operator[](size_t i) const;

        Reference_t back();

        [[nodiscard]] ConstReference_t back() const;

        [[nodiscard]] size_t get_dims() const override;

        [[nodiscard]] size_t get_dims_bytes() const override;

        [[nodiscard]] size_t get_size() const override;

        [[nodiscard]] size_t get_size_bytes() const override;

        [[nodiscard]] size_t get_capacity() const override;

        T *get_ptr();

        [[nodiscard]] const T *get_ptr() const;

        [[nodiscard]] const void *get_void_ptr() const override;

        void swap(size_t i0, size_t i1) override;

        [[nodiscard]] bool is_empty() const override;

        void clear() override;

        void reset(const T &t = T());

        void free_unused_memory() override;

        void resize(size_t n) override;

        void reserve(size_t n) override;

        void push_back() override;

        void push_back(const T &value);

        [[nodiscard]] const T &default_value() const;

        Iterator_t begin();

        Iterator_t end();

        [[nodiscard]] ConstIterator_t begin() const;

        [[nodiscard]] ConstIterator_t end() const;

        [[nodiscard]] std::string info() const override;

        [[nodiscard]] std::string to_string() const override;

        [[nodiscard]] std::string to_string(size_t idx) const override;

        void set_column_names(std::string column_names);

        Storage_t &storage();

        const Storage_t &storage() const;

    protected:
        std::string m_column_names;
        T m_default_value;
        Storage_t m_storage;
    };


    template<typename T>
    PropertyVector<T>::PropertyVector(std::string name, T t) : AbstractProperty(std::move(name)), m_default_value(t) {}

    template<typename T>
    typename PropertyVector<T>::Reference_t PropertyVector<T>::operator[](size_t i) { return m_storage[i]; }

    template<typename T>
    typename PropertyVector<T>::ConstReference_t PropertyVector<T>::operator[](size_t i) const { return m_storage[i]; }

    template<typename T>
    typename PropertyVector<T>::Reference_t PropertyVector<T>::back() {
        return m_storage.back();
    }

    template<typename T>
    typename PropertyVector<T>::ConstReference_t PropertyVector<T>::back() const {
        return m_storage.back();
    }

    template<typename T, std::enable_if_t<std::is_fundamental_v<T>, size_t> = 0>
    inline size_t GetDimsensions(const T &) {
        return size_t(1);
    }

    template<typename T, std::enable_if_t<IsEigenType<T>, size_t> = 0>
    inline size_t GetDimsensions(const Eigen::EigenBase<T> &value) {
        return size_t(value.size());
    }

    template<typename T>
    size_t PropertyVector<T>::get_dims() const {
        return GetDimsensions(m_default_value);
    }

    template<typename T, std::enable_if_t<IsEigenType<T>, size_t> = 0>
    inline size_t GetDimsensionsBytes(const Eigen::EigenBase<T> &value) {
        return value.size() * sizeof(typename T::Scalar);
    }

    template<typename T, std::enable_if_t<std::is_fundamental_v<T>, size_t> = 0>
    inline size_t GetDimsensionsBytes(const T &value) {
        return sizeof(value);
    }

    template<typename T>
    size_t PropertyVector<T>::get_dims_bytes() const {
        return GetDimsensionsBytes<T>(m_default_value);
    }

    template<typename T>
    size_t PropertyVector<T>::get_size() const { return m_storage.size(); }

    template<typename T>
    size_t PropertyVector<T>::get_size_bytes() const { return get_size() * get_dims_bytes(); }

    template<typename T>
    size_t PropertyVector<T>::get_capacity() const { return m_storage.capacity(); }

    template<typename T>
    T *PropertyVector<T>::get_ptr() { return m_storage.data(); }

    template<typename T>
    const T *PropertyVector<T>::get_ptr() const { return m_storage.data(); }

    template<typename T>
    const void *PropertyVector<T>::get_void_ptr() const { return static_cast<const void *>(get_ptr()); }

    template<typename T>
    void PropertyVector<T>::swap(size_t i0, size_t i1) {
        auto tmp = m_storage[i0];
        m_storage[i0] = m_storage[i1];
        m_storage[i1] = tmp;
        set_dirty();
    }

    template<typename T>
    bool PropertyVector<T>::is_empty() const { return m_storage.empty(); }

    template<typename T>
    void PropertyVector<T>::clear() {
        m_storage.clear();
        set_dirty();
    }

    template<typename T>
    void PropertyVector<T>::reset(const T &t) {
        for (auto &item: m_storage) {
            item = t;
        }
        set_dirty();
    }

    template<typename T>
    void PropertyVector<T>::free_unused_memory() {
        m_storage.shrink_to_fit();
        set_dirty();
    }

    template<typename T>
    void PropertyVector<T>::resize(size_t n) {
        if (n > get_size()) {
            for (size_t i = get_size(); i < n; ++i) {
                push_back();
            }
        } else {
            m_storage.resize(n);
        }
        free_unused_memory();
        set_dirty();
    }

    template<typename T>
    void PropertyVector<T>::reserve(size_t n) { m_storage.reserve(n); }

    template<typename T>
    void PropertyVector<T>::push_back() {
        m_storage.push_back(m_default_value);
        set_dirty();
    }

    template<typename T>
    void PropertyVector<T>::push_back(const T &value) {
        m_storage.push_back(value);
        set_dirty();
    }

    template<typename T>
    const T &PropertyVector<T>::default_value() const { return m_default_value; }

    template<typename T>
    typename PropertyVector<T>::Iterator_t PropertyVector<T>::begin() {
        return m_storage.begin();
    }

    template<typename T>
    typename PropertyVector<T>::Iterator_t PropertyVector<T>::end() {
        return m_storage.end();
    }

    template<typename T>
    typename PropertyVector<T>::ConstIterator_t PropertyVector<T>::begin() const {
        return m_storage.cbegin();
    }

    template<typename T>
    typename PropertyVector<T>::ConstIterator_t PropertyVector<T>::end() const {
        return m_storage.cend();
    }

    template<typename T>
    void PropertyVector<T>::set_column_names(std::string column_names) {
        m_column_names = std::move(column_names);
    }

    template<typename T>
    std::string PropertyVector<T>::to_string(size_t idx) const {
        std::stringstream ss;
        return Print(ss, m_storage[idx]).str();
    }

    template<typename T>
    std::string PropertyVector<T>::info() const {
        std::stringstream ss;
        ss << "PropertyName: " << m_name << "\n";
        ss << "        Size: " << get_size() << "\n";
        ss << "        Dims: " << get_dims() << "\n";
        ss << "default_value: ";
        Print(ss, m_default_value) << "\n\n";
        ss << "    Contents:\n";
        ss << m_column_names << "\n";
        return ss.str();
    }

    template<typename T>
    std::string PropertyVector<T>::to_string() const {
        std::stringstream ss;
        for (size_t i = 0; i < get_size(); ++i) {
            Print(ss, m_storage[i]) << "\n";
        }
        return ss.str();
    }

    template<typename T>
    PropertyVector<T>::Storage_t &PropertyVector<T>::storage() {
        return m_storage;
    }

    template<typename T>
    const PropertyVector<T>::Storage_t &PropertyVector<T>::storage() const {
        return m_storage;
    }

    // specialization for bool properties
    template<>
    inline const bool *PropertyVector<bool>::get_ptr() const {
        return nullptr;
    }

    // specialization for bool properties
    template<>
    inline bool *PropertyVector<bool>::get_ptr() {
        return nullptr;
    }

}

#endif //ENGINE23_PROPERTYVECTOR_H
