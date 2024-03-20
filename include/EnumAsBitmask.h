//
// Created by alex on 20.03.24.
//

#ifndef ENGINE23_ENUMASBITMASK_H
#define ENGINE23_ENUMASBITMASK_H

/*  Usage:
 *  enum class Categories64Bits{
 *      None=0,
 *      One=Bit(0),
 *      Two=Bit(1),
 *      ...
 *      SixtyFour=Bit(63)
 *  }
 *
 *  ENABLE_BITMASK_OPERATORS(Categories64Bits);
 *
 *  Categories64Bits maks = One | Two | ... | Five;
 * */

#include <cmath>
#include <sstream>

namespace Bcg {

#define BIT(x) (1 << x)
#define ENABLE_BITMASK_OPERATORS(x)  \
template<>                           \
struct EnableBitMaskOperators<x>     \
{                                    \
    static const bool enable = true; \
};


    template<typename Enum>
    struct EnableBitMaskOperators {
        static const bool enable = false;
    };

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, bool>::type
    operator==(Enum lhs, Enum rhs) {
        return static_cast<Enum> (
                static_cast<typename std::underlying_type<Enum>::type>(lhs) ==
                static_cast<typename std::underlying_type<Enum>::type>(rhs)
        );
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
    operator|(Enum lhs, Enum rhs) {
        return static_cast<Enum> (
                static_cast<typename std::underlying_type<Enum>::type>(lhs) |
                static_cast<typename std::underlying_type<Enum>::type>(rhs)
        );
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
    operator&(Enum lhs, Enum rhs) {
        return static_cast<Enum> (
                static_cast<typename std::underlying_type<Enum>::type>(lhs) &
                static_cast<typename std::underlying_type<Enum>::type>(rhs)
        );
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
    operator^(Enum lhs, Enum rhs) {
        return static_cast<Enum> (
                static_cast<typename std::underlying_type<Enum>::type>(lhs) ^
                static_cast<typename std::underlying_type<Enum>::type>(rhs)
        );
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
    operator~(Enum rhs) {
        return static_cast<Enum> (
                ~static_cast<typename std::underlying_type<Enum>::type>(rhs)
        );
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
    &operator|=(Enum &lhs, Enum rhs) {
        lhs = static_cast<Enum> (
                static_cast<typename std::underlying_type<Enum>::type>(lhs) |
                static_cast<typename std::underlying_type<Enum>::type>(rhs)
        );

        return lhs;
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type &
    operator&=(Enum &lhs, Enum rhs) {
        lhs = static_cast<Enum> (
                static_cast<typename std::underlying_type<Enum>::type>(lhs) &
                static_cast<typename std::underlying_type<Enum>::type>(rhs)
        );

        return lhs;
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type &
    operator^=(Enum &lhs, Enum rhs) {
        lhs = static_cast<Enum> (
                static_cast<typename std::underlying_type<Enum>::type>(lhs) ^
                static_cast<typename std::underlying_type<Enum>::type>(rhs)
        );

        return lhs;
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type &
    SET(Enum &lhs, Enum rhs) {
        return lhs |= rhs;
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type &
    CLEAR(Enum &lhs, Enum rhs) {
        return lhs &= ~rhs;
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type &
    CLEAR_ALL(Enum &lhs) {
        return lhs = static_cast<Enum>(0);
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type &
    SET_ALL(Enum &lhs) {
        return lhs = static_cast<Enum>(~0);
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, bool>::type
    TEST(Enum lhs, Enum rhs) {
        return static_cast<bool>(lhs & rhs);
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, bool>::type
    CONTAINS(Enum lhs, Enum rhs) {
        return static_cast<bool>((lhs & rhs) == rhs);
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, typename std::underlying_type<Enum>::type>::type
    RMSB(Enum value) {
        auto x = static_cast<typename std::underlying_type<Enum>::type>(value);
        return x - (x & (x - 1));
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, double>::type
    POSRMSB(Enum value) {
        return std::log2(RMSB(value));
    }

    template<typename Enum>
    typename std::enable_if<EnableBitMaskOperators<Enum>::enable, std::string>::type
    TO_STRING_BITS(Enum value, size_t size) {
        std::stringstream ss;
        for (size_t bit = 0; bit < size; ++bit) {
            ss << TEST(value, static_cast<Enum>(BIT(bit)));
        }
        return ss.str();
    }

}

#endif //ENGINE23_ENUMASBITMASK_H
