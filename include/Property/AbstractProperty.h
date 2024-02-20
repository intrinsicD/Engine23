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
}


#endif //ENGINE23_ABSTRACTPROPERTY_H
