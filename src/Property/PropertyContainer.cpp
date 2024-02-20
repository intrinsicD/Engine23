//
// Created by alex on 20.02.24.
//

#include "PropertyContainer.h"

namespace Bcg{

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

    std::shared_ptr<AbstractProperty> PropertyContainer::get(const std::string &property_name) {
        auto iter = m_storage.find(property_name);
        if(iter == m_storage.end()){
            return nullptr;
        }else{
            return iter->second;
        }
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

    PropertyContainer::Storage_t &PropertyContainer::storage() {
        return m_storage;
    }

    const PropertyContainer::Storage_t &PropertyContainer::storage() const {
        return m_storage;
    }

    std::ostream &operator<<(std::ostream &stream, const PropertyContainer &container) {
        for (const auto &items: container.storage()) {
            stream << items.second->to_string() << "\n";
        }
        return stream;
    }
}