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
        using Iterator_t = Storage_t::iterator;
        using ConstIterator_t = Storage_t::const_iterator;

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

        Iterator_t begin() {
            return m_storage.begin();
        }

        Iterator_t end() {
            return m_storage.end();
        }

        ConstIterator_t begin() const {
            return m_storage.begin();
        }

        ConstIterator_t end() const {
            return m_storage.end();
        }

        template<typename HandleType = HandleBase<size_t>, class Container = PropertyContainer>
        auto &&indices() {
            return IndicesIterator<HandleType, Container>(*this);
        }

    protected:
        std::string m_name;
        Storage_t m_storage;
    };

    PropertyContainer::PropertyContainer(std::string name) : m_name(std::move(name)), m_storage() {}

    const std::string &PropertyContainer::get_name() const {
        return m_name;
    }

    bool PropertyContainer::has(const std::string &property_name) const {
        auto iter = m_storage.find(property_name);
        if (iter == m_storage.end()) {
            return false;
        } else {
            return true;
        }
    }

    template<typename T>
    bool PropertyContainer::has(Property<T> property) const {
        return has(property.get_name());
    }

    std::shared_ptr<AbstractProperty> PropertyContainer::get(const std::string &property_name) {
        auto iter = m_storage.find(property_name);
        if(iter == m_storage.end()){
            return nullptr;
        }else{
            return iter->second;
        }
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

    void PropertyContainer::clear() {
        std::for_each(m_storage.begin(), m_storage.end(), [](auto &p) {
            p.second->clear();
        });
    }

    bool PropertyContainer::empty() const {
        return m_storage.empty();
    }

    bool PropertyContainer::erase(const std::string &property_name) {
        auto iter = m_storage.find(property_name);
        if (iter != m_storage.end()) {
            m_storage.erase(iter);
            return true;
        }
        return false;
    }

    template<typename T>
    bool PropertyContainer::erase(Property<T> property) {
        return erase(property.get_name());
    }

    void PropertyContainer::swap(size_t i0, size_t i1) {
        std::for_each(m_storage.begin(), m_storage.end(), [i0, i1](auto &p) {
            p.second->swap(i0, i1);
        });
    }

    void PropertyContainer::free_unused_memory() {
        std::for_each(m_storage.begin(), m_storage.end(), [](auto &p) {
            p.second->free_unused_memory();
        });
    }

    void PropertyContainer::resize(size_t n) {
        std::for_each(m_storage.begin(), m_storage.end(), [&n](auto &p) {
            p.second->resize(n);
        });
    }

    void PropertyContainer::reserve(size_t n) {
        std::for_each(m_storage.begin(), m_storage.end(), [&n](auto &p) {
            p.second->reserve(n);
        });
    }

    void PropertyContainer::push_back() {
        std::for_each(m_storage.begin(), m_storage.end(), [](auto &p) {
            p.second->push_back();
        });
    }

    void PropertyContainer::set_dirty() {
        std::for_each(m_storage.begin(), m_storage.end(), [](auto &p) {
            p.second->set_dirty();
        });
    }

    size_t PropertyContainer::get_size() const {
        return empty() ? 0 : m_storage.begin()->second->get_size();
    }

    bool PropertyContainer::has_dirty_properties() const {
        return std::any_of(m_storage.begin(), m_storage.end(), [](const auto &p) {
            return p.second->is_dirty();
        });
    }

    std::vector<std::string>
    PropertyContainer::get_property_names(const std::function<bool(const std::shared_ptr<AbstractProperty> &)> &filter_function) const {
        std::vector<std::string> names;
        if(filter_function){
            std::for_each(m_storage.begin(), m_storage.end(), [&names, &filter_function](const auto &p) {
                if(filter_function(p.second)){
                    names.push_back(p.first);
                }
            });
        }else{
            std::for_each(m_storage.begin(), m_storage.end(), [&names](const auto &p) {
                names.push_back(p.first);
            });
        }
        return names;
    }

    std::vector<std::string> PropertyContainer::get_dirty_property_names() const {
        std::vector<std::string> names;
        std::for_each(m_storage.begin(), m_storage.end(), [&names](const auto &p) {
            if (p.second->is_dirty()) {
                names.push_back(p.second->get_name());
            }
        });
        return names;
    }

    size_t PropertyContainer::num_properties() const {
        return m_storage.size();
    }

    bool PropertyContainer::is_valid(size_t handle) const{
        return handle < get_size();
    }

    std::shared_ptr<AbstractProperty> PropertyContainer::operator[](const std::string &property_name) const{
        return m_storage.at(property_name);
    }

    inline std::ostream &operator<<(std::ostream &stream, const PropertyContainer &container) {
        for (const auto &items: container) {
            stream << items.second->to_string() << "\n";
        }
        return stream;
    }
}

#endif //ENGINE23_PROPERTYCONTAINER_H
