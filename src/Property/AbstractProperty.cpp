//
// Created by alex on 20.02.24.
//

#include "AbstractProperty.h"

namespace Bcg{
    AbstractProperty::AbstractProperty(std::string name) : m_name(std::move(name)) {}

    [[nodiscard]] const std::string &AbstractProperty::get_name() const { return m_name; }

    void AbstractProperty::set_dirty() { m_is_dirty = true; }

    void AbstractProperty::set_clean() { m_is_dirty = false; }

    [[nodiscard]] bool AbstractProperty::is_dirty() const { return m_is_dirty; }

    void AbstractProperty::update_string_cache() {
        string_cache.resize(get_size());
        for (size_t i = 0; i < get_size(); ++i) {
            string_cache[i] = to_string(i);
        }
    }

    const char *AbstractProperty::get_cached_string(int idx) const {
        return string_cache[idx].c_str();
    }

    inline std::ostream &operator<<(std::ostream &stream, const AbstractProperty &a_property) {
        stream << a_property.to_string() << "\n";
        return stream;
    }
}