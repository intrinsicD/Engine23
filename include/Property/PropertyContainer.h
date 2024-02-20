//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_PROPERTYCONTAINER_H
#define ENGINE23_PROPERTYCONTAINER_H

#include "Property.h"
#include "PropertyIterator.h"

namespace Bcg {

    class PropertyContainer {
    public:
        using Storage_t = std::unordered_map<std::string, std::shared_ptr<AbstractProperty>>;

        PropertyContainer() = default;

        explicit PropertyContainer(std::string name);

        virtual ~PropertyContainer() = default;

        const std::string &get_name() const;

        bool has(const std::string &property_name) const;

        template<typename T>
        bool has(Property<T> property) const;

        std::shared_ptr<AbstractProperty> get(const std::string &property_name);

        template<typename T>
        Property<T> get(const std::string &property_name);

        template<typename T>
        Property<T> get(const std::string &property_name) const;

        template<typename T>
        Property<T> add(const std::string &property_name, T t = T());

        template<typename T>
        Property<T> get_or_add(const std::string &property_name, T t = T());

        virtual void clear();

        bool empty() const;

        bool erase(const std::string &property_name);

        template<typename T>
        bool erase(Property<T> property);

        void swap(size_t i0, size_t i1);

        void free_unused_memory();

        void resize(size_t n);

        void reserve(size_t n);

        void push_back();

        void set_dirty();

        size_t get_size() const;

        bool has_dirty_properties() const;

        std::vector<std::string>
        get_property_names(
                const std::function<bool(const std::shared_ptr<AbstractProperty> &)> &filter_function = nullptr) const;

        std::vector<std::string> get_dirty_property_names() const;

        size_t num_properties() const;

        bool is_valid(size_t handle) const;

        std::shared_ptr<AbstractProperty> operator[](const std::string &name) const;

        Storage_t &storage();

        const Storage_t &storage() const;

        friend std::ostream &operator<<(std::ostream &stream, const PropertyContainer &container);

    protected:
        std::string m_name;
        Storage_t m_storage;
    };

    template<typename T>
    bool PropertyContainer::has(Property<T> property) const {
        return has(property.get_name());
    }

    template<typename T>
    Property<T> PropertyContainer::get(const std::string &property_name) {
        auto iter = m_storage.find(property_name);
        if (iter == m_storage.end()) {
            return {};
        } else {
            return Property<T>(std::dynamic_pointer_cast<PropertyVector<T>>(iter->second));
        }
    }

    template<typename T>
    Property<T> PropertyContainer::get(const std::string &property_name) const {
        auto iter = m_storage.find(property_name);
        if (iter == m_storage.end()) {
            return {};
        } else {
            return Property<T>(std::dynamic_pointer_cast<PropertyVector<T>>(iter->second));
        }
    }

    template<typename T>
    Property<T> PropertyContainer::add(const std::string &property_name, T t) {
        auto iter = m_storage.find(property_name);
        if (iter == m_storage.end()) {
            auto sptr = std::make_shared<PropertyVector<T>>(property_name, t);
            if (sptr->get_size() != get_size()) {
                sptr->resize(get_size());
            }
            m_storage[property_name] = sptr;
            return Property<T>(sptr);
        } else {
            return Property<T>(std::dynamic_pointer_cast<PropertyVector<T>>(iter->second));
        }
    }

    template<typename T>
    Property<T> PropertyContainer::get_or_add(const std::string &property_name, T t) {
        auto property = get<T>(property_name);
        if (property) return property;
        else return add<T>(property_name, t);
    }

    template<typename T>
    bool PropertyContainer::erase(Property<T> property) {
        return erase(property.get_name());
    }
}

#endif //ENGINE23_PROPERTYCONTAINER_H
