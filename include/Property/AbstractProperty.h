//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_ABSTRACTPROPERTY_H
#define ENGINE23_ABSTRACTPROPERTY_H

#include <vector>
#include <string>
#include <ostream>

namespace Bcg {
    class PropertyContainer;

    class AbstractProperty {
    public:
        AbstractProperty() = default;

        explicit AbstractProperty(std::string name);

        virtual ~AbstractProperty() = default;

        [[nodiscard]] const std::string &get_name() const;

        [[nodiscard]] virtual size_t get_dims() const = 0;

        [[nodiscard]] virtual size_t get_dims_bytes() const = 0;

        [[nodiscard]] virtual size_t get_size() const = 0;

        [[nodiscard]] virtual size_t get_size_bytes() const = 0;

        [[nodiscard]] virtual size_t get_capacity() const = 0;

        [[nodiscard]] virtual const void *get_void_ptr() const = 0;

        virtual void swap(size_t i0, size_t i1) = 0;

        void set_dirty();

        void set_clean();

        [[nodiscard]] bool is_dirty() const;

        [[nodiscard]] virtual bool is_empty() const = 0;

        virtual void clear() = 0;

        virtual void free_unused_memory() = 0;

        virtual void resize(size_t n) = 0;

        virtual void reserve(size_t n) = 0;

        virtual void push_back() = 0;

        [[nodiscard]] virtual std::string info() const = 0;

        [[nodiscard]] virtual std::string to_string() const = 0;

        [[nodiscard]] virtual std::string to_string(size_t idx) const = 0;

        void update_string_cache();

        [[nodiscard]] const char *get_cached_string(int idx) const;

    protected:
        friend PropertyContainer;

        std::string m_name;
        bool m_is_dirty = false;

    private:
        std::vector<std::string> string_cache;
    };

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


#endif //ENGINE23_ABSTRACTPROPERTY_H
