//
// Created by alex on 22.03.24.
//

#ifndef ENGINE23_DYNAMICBITSET_H
#define ENGINE23_DYNAMICBITSET_H

#include <vector>
#include <limits>
#include <string>
#include <algorithm>
#include <bitset>

namespace Bcg {
    class DynamicBitset {
    public:
        struct reference_t {
            size_t pos;
            DynamicBitset *dbs;

            reference_t(size_t pos, DynamicBitset *dbs) : pos(pos), dbs(dbs) {}

            reference_t &operator=(bool val) {
                val ? dbs->set(pos) : dbs->reset(pos);
                return *this;
            }

            operator bool() const noexcept {
                return dbs->test(pos);
            }
        };

        struct const_reference_t {
            size_t pos;
            const DynamicBitset *dbs;

            const_reference_t(size_t pos, const DynamicBitset *dbs) : pos(pos), dbs(dbs) {}

            operator bool() const noexcept {
                return dbs->test(pos);
            }
        };

        DynamicBitset() : m_set_bits(0), m_storage(1, 0) {};

        explicit DynamicBitset(size_t number) : m_set_bits(0), m_storage(1, 0) {
            for (size_t i = 0; i < 64; ++i) {
                if (CHECK_BIT(number, i)) {
                    set(i);
                }
            }
        };

        explicit DynamicBitset(std::string number, char one = '1') : m_set_bits(0), m_storage(1, 0) {
            for (size_t i = 0; i < number.size(); ++i) {
                if (number[i] == one) {
                    set(i);
                }
            }
        }

        void clear() {
            m_set_bits = 0;
            m_storage.clear();
        }

        [[nodiscard]] std::string to_string() const {
            std::string number;
            size_t is = item_size();
            for (const auto &item: m_storage) {
                for (size_t i = 0; i < is; ++i) {
                    if (CHECK_BIT(item, i)) {
                        number += '1';
                    } else {
                        number += '0';
                    }
                }
            }
            number.erase(std::find_if(number.rbegin(), number.rend(), [](int ch) {
                return ch != '0';
            }).base(), number.end());
            return number;
        }

        bool operator==(const DynamicBitset &other) const {
            size_t cap = std::min(capacity(), other.capacity());

            for (size_t i = 0; i < cap; ++i) {
                if ((*this)[i] != other[i]) return false;
            }

            if (other.capacity() > cap) {
                return other.is_zero_from(cap);
            }
            return is_zero_from(cap);
        }

        bool operator!=(const DynamicBitset &other) const {
            size_t cap = std::min(capacity(), other.capacity());

            for (size_t i = 0; i < cap; ++i) {
                if ((*this)[i] != other[i]) return true;
            }

            if (other.capacity() > cap) {
                return other.is_not_zero_from(cap);
            }
            return is_not_zero_from(cap);
        }

        [[nodiscard]] bool is_zero_from(size_t pos) const {
            size_t cap = capacity();
            for (size_t i = pos; i < cap; ++i) {
                if ((*this)[i] == 1) return false;
            }
            return true;
        }

        [[nodiscard]] bool is_not_zero_from(size_t pos) const {
            size_t cap = capacity();
            for (size_t i = pos; i < cap; ++i) {
                if ((*this)[i] == 1) return true;
            }
            return false;
        }

        [[nodiscard]] bool all() const {
            return std::all_of(m_storage.begin(), m_storage.end(), [&](const auto i) {
                return i;
            });
        }

        [[nodiscard]] bool all_of(const std::vector<size_t> &indices) const {
            size_t cap = capacity();
            return std::all_of(indices.begin(), indices.end(), [&](const auto i) {
                return i < cap && test(i);
            });
        }

        [[nodiscard]] bool any() const {
            return std::all_of(m_storage.begin(), m_storage.end(), [](const auto i) {
                return i != 0;
            });
        }

        [[nodiscard]] bool none() const {
            return std::all_of(m_storage.begin(), m_storage.end(), [](const auto i) {
                return i == 0;
            });
        }

        [[nodiscard]] size_t count() const {
            size_t count = 0;
            for (const auto i: m_storage) {
                count += COUNTSETBITS(i);
            }
            return count;
        }

        void resize(size_t s) {
            while (capacity() <= s) {
                m_storage.push_back(0);
            }
            m_storage.shrink_to_fit();
        }

        bool set(size_t i) {
            if (pos_too_large(i)) {
                resize(i);
            }
            size_t vi = vector_index(i);
            size_t ii = item_index(i);
            if (test(vi, ii)) {
                return false;
            }
            SET_BIT(m_storage[vi], ii);
            ++m_set_bits;
            return true;
        }

        bool reset(size_t i) {
            if (pos_too_large(i)) {
                resize(i);
            }
            size_t vi = vector_index(i);
            size_t ii = item_index(i);
            if (test(vi, ii)) {
                RESET_BIT(m_storage[vi], ii);
                --m_set_bits;
                return true;
            }
            return false;
        }

        [[nodiscard]] bool test(size_t i) const {
            if (i >= capacity()) return false;
            return test(vector_index(i), item_index(i));
        }

        reference_t operator[](size_t i) { return {i, this}; }

        const_reference_t operator[](size_t i) const { return {i, this}; }

        static constexpr size_t item_size() noexcept {
            return std::numeric_limits<item_t>::digits;
        }

        [[nodiscard]] size_t num_set_bits() const { return m_set_bits; }

        [[nodiscard]] size_t size() const {
            return m_storage.size();
        }

        [[nodiscard]] size_t capacity() const {
            return size() * item_size();
        }

        DynamicBitset &operator&=(const DynamicBitset &other) {
            return binary_operator_helper(*this, other, std::bit_and<>());
        }

        DynamicBitset &operator|=(const DynamicBitset &other) {
            return binary_operator_helper(*this, other, std::bit_or<>());
        }

        DynamicBitset &operator^=(const DynamicBitset &other) {
            return binary_operator_helper(*this, other, std::bit_xor<>());
        }

        DynamicBitset operator&(const DynamicBitset &other) const {
            if (size() < other.size()) {
                DynamicBitset new_bitset = *this;
                return new_bitset.operator&=(other);
            } else {
                DynamicBitset new_bitset = other;
                return new_bitset.operator&=(*this);
            }
        }

        DynamicBitset operator|(const DynamicBitset &other) const {
            if (size() < other.size()) {
                DynamicBitset new_bitset = *this;
                return new_bitset.operator|=(other);
            } else {
                DynamicBitset new_bitset = other;
                return new_bitset.operator|=(*this);
            }
        }

        DynamicBitset operator^(const DynamicBitset &other) const {
            if (size() < other.size()) {
                DynamicBitset new_bitset = *this;
                return new_bitset.operator^=(other);
            } else {
                DynamicBitset new_bitset = other;
                return new_bitset.operator^=(*this);
            }
        }

        DynamicBitset operator~() const {
            DynamicBitset new_bitset = *this;
            return new_bitset.flip();
        }

        DynamicBitset operator<<(size_t pos) const {
            size_t cap = capacity();
            DynamicBitset new_bitset;
            new_bitset.resize(cap - pos);

            for (size_t i = 0; i < new_bitset.capacity() - pos; ++i) {
                new_bitset[i] = test(i + pos);
            }

            return new_bitset;
        }

        DynamicBitset &operator<<=(size_t pos) {
            *this = operator<<(pos);
            return *this;
        }

        DynamicBitset operator>>(size_t pos) const {
            DynamicBitset new_bitset;
            new_bitset.resize(capacity() + pos);

            for (size_t i = 0; i < capacity() - pos; ++i) {
                new_bitset[i + pos] = test(i);
            }
            return new_bitset;
        }

        DynamicBitset &operator>>=(size_t pos) {
            *this = operator>>(pos);
            return *this;
        }

        DynamicBitset &flip() {
            for (size_t i = 0; i < capacity(); ++i) {
                (*this)[i] = !(*this)[i];
            }
            return *this;
        }

        DynamicBitset &reset() {
            for (auto &i: m_storage) {
                i = 0;
            }
            return *this;
        }

        DynamicBitset &flip(size_t pos) {
            reference_t(pos, this) = !reference_t(pos, this);
            return *this;
        }

    private:
        using item_t = unsigned char;

        template<typename T>
        [[nodiscard]] inline T BIT(T pos) const {
            return 1 << pos;
        }

        template<typename T, typename U>
        inline void SET_BIT(T &var, U pos) {
            var |= BIT(pos);
        }

        template<typename T, typename U>
        [[nodiscard]] inline T DROP(T var, U pos) const {
            return var >> pos;
        }

        template<typename T, typename U>
        [[nodiscard]] inline bool CHECK_BIT(T var, U pos) const {
            return DROP(var, pos) & 1;
        }

        template<typename T, typename U>
        inline void RESET_BIT(T &var, U pos) {
            var &= ~BIT(pos);
        }

        template<typename T>
        [[nodiscard]] size_t COUNTSETBITS(T var) const {
            //return std::bitset<sizeof(size_t) * 8>(var).count();
            size_t count = 0;
            while (var != 0) {
                count += var & 1;
                var >>= 1;
            }
            return count;
        }

        template<>
        [[nodiscard]] size_t COUNTSETBITS<uint64_t>(uint64_t var) const {
            if constexpr (sizeof(size_t) == 8 /*  sizeof(size_t) * 8 = 64*/) {
                var = var - ((var >> 1) & 0x5555555555555555UL);
                var = (var & 0x3333333333333333UL) + ((var >> 2) & 0x3333333333333333UL);
                return (size_t) ((((var + (var >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
            } else {
                var = var - ((var >> 1) & 0x55555555);
                var = (var & 0x33333333) + ((var >> 2) & 0x33333333);
                return ((var + (var >> 4)) & 0x0F0F0F0F) * 0x01010101 >> 24;
            }
        }

        [[nodiscard]] inline bool pos_too_large(size_t pos) const {
            return pos >= capacity();
        }

        [[nodiscard]] inline bool test(size_t vi, size_t ii) const {
            return CHECK_BIT(m_storage[vi], ii);
        }

        static inline size_t vector_index(size_t pos) noexcept { return pos / item_size(); }

        static inline size_t item_index(size_t pos) noexcept { return pos % item_size(); }

        // Helper function to reduce code duplication in operator&=, operator|=, and operator^=
        template<typename BinaryOperation>
        DynamicBitset &binary_operator_helper(DynamicBitset &lhs, const DynamicBitset &rhs, BinaryOperation op) {
            if (rhs.size() > lhs.size()) {
                lhs.resize(rhs.capacity());
            }
            size_t s = std::min(lhs.size(), rhs.size());
            for (size_t i = 0; i < s; ++i) {
                lhs.m_storage[i] = op(lhs.m_storage[i], rhs.m_storage[i]);
            }
            return lhs;
        }

        size_t m_set_bits;
        std::vector<item_t> m_storage;
    };

}
#endif //ENGINE23_DYNAMICBITSET_H
