#ifndef DSA_UTILITY_HPP
#define DSA_UTILITY_HPP

#include <type_traits>
#include <limits>

namespace dsa::utility {
    namespace traits {
        template<typename T, typename = void>
        struct has_lower_bound : std::false_type {};

        template<typename T>
        struct has_lower_bound<T, std::void_t<decltype(T::lower_bound())>>
            : std::is_convertible<decltype(T::lower_bound()), T> {};

        template<typename T, typename = void>
        struct has_upper_bound : std::false_type {};

        template<typename T>
        struct has_upper_bound<T, std::void_t<decltype(T::upper_bound())>>
            : std::is_convertible<decltype(T::upper_bound()), T> {};

        template<typename T>
        inline constexpr bool has_lower_bound_v = has_lower_bound<T>::value;

        template<typename T>
        inline constexpr bool has_upper_bound_v = has_upper_bound<T>::value;
    } // namespace traits

    /**
     * Monoid ops for range-query structures. Each struct provides identity() and combine().
     * A monoid (S, ·, e) satisfies: associativity (a·b)·c = a·(b·c), identity e·a = a·e = a.
     *
     * Flags:
     *   is_idemp            — idempotent  : combine(a, a) = a  ∀a
     *   is_selective      — selective   : combine(a, b) ∈ {a, b}  ∀a,b
     *   is_invertible       — invertible  : ∀a ∃a⁻¹ s.t. a·a⁻¹ = e  (group axiom)
     *   is_range_add_compat — shift-covariant: ∃f s.t. combine(a₁+d,…,aₙ+d) = f(combine(a₁,…,aₙ), d, n)
     *                           where f(s,d,n) = s+d·n  (sum),  f(s,d,n) = s+d  (max/min)
     *
     * | ops | idemp | comp | invert | range_add | identity |
     * |-----|-------|------|--------|-----------|----------|
     * | sum |       |      |   ✓    |     ✓     | 0        |
     * | mul |       |      |        |           | 1        |
     * | max |   ✓   |  ✓   |        |     ✓     | -∞       |
     * | min |   ✓   |  ✓   |        |     ✓     | +∞       |
     * | gcd |   ✓   |      |        |           | 0        |
     * | lcm |   ✓   |      |        |           | 1        |
     * | or  |   ✓   |      |        |           | 0        |
     * | and |   ✓   |      |        |           | ~0       |
     */
    namespace monoid {
        template<typename ValueType>
        struct sumops {
            static constexpr bool is_invertible        = true;
            static constexpr bool is_range_add_compat  = true;
            static constexpr ValueType identity(){ return ValueType(0); }
            static ValueType combine(const ValueType& a, const ValueType& b){ return a + b; }
            static ValueType inverse(const ValueType& info, const ValueType& old_val, const ValueType& new_val){ return info - old_val + new_val; }
        };

        template<typename ValueType>
        struct mulops {
            static constexpr ValueType identity(){ return ValueType(1); }
            static ValueType combine(const ValueType& a, const ValueType& b){ return a * b; }
        };

        template<typename ValueType>
        struct maxops {
            static constexpr bool is_idemp             = true;
            static constexpr bool is_selective        = true;
            static constexpr bool is_range_add_compat  = true;
            static constexpr ValueType identity(){
                if constexpr (std::is_arithmetic_v<ValueType>) {
                    return std::numeric_limits<ValueType>::lowest();
                } else {
                    static_assert(traits::has_lower_bound_v<ValueType>,
                        "maxops<ValueType>: non-arithmetic ValueType must provide a "
                        "static constexpr lower_bound() returning its identity");
                    return ValueType::lower_bound();
                }
            }
            static ValueType combine(const ValueType& a, const ValueType& b){ return (a > b) ? a : b; }
        };

        template<typename ValueType>
        struct minops {
            static constexpr bool is_idemp             = true;
            static constexpr bool is_selective        = true;
            static constexpr bool is_range_add_compat  = true;
            static constexpr ValueType identity(){
                if constexpr (std::is_arithmetic_v<ValueType>) {
                    return std::numeric_limits<ValueType>::max();
                } else {
                    static_assert(traits::has_upper_bound_v<ValueType>,
                        "minops<ValueType>: non-arithmetic ValueType must provide a "
                        "static constexpr upper_bound() returning its identity");
                    return ValueType::upper_bound();
                }
            }
            static ValueType combine(const ValueType& a, const ValueType& b){ return (a < b) ? a : b; }
        };

        template<typename ValueType>
        struct gcdops {
            static constexpr bool is_idemp = true;
            static constexpr ValueType identity(){ return ValueType(0); }
            static ValueType combine(const ValueType& a, const ValueType& b){
                ValueType c = a;
                ValueType d = b;
                while (d != ValueType(0)){
                    ValueType r = c % d;
                    c = d;
                    d = r;
                }
                return c;
            }
        };

        template<typename ValueType>
        struct lcmops {
            static constexpr bool is_idemp = true;
            static constexpr ValueType identity(){ return ValueType(1); }
            static ValueType combine(const ValueType& a, const ValueType& b){
                return (a * b) / gcdops<ValueType>::combine(a, b);
            }
        };

        template<typename ValueType>
        struct orops {
            static constexpr bool is_idemp = true;
            static constexpr ValueType identity(){ return ValueType(0); }
            static ValueType combine(const ValueType& a, const ValueType& b){ return a | b; }
        };

        template<typename ValueType>
        struct andops {
            static constexpr bool is_idemp = true;
            static constexpr ValueType identity(){ return ValueType(1); }
            static ValueType combine(const ValueType& a, const ValueType& b){ return a & b; }
        };

        template<typename O, typename W, typename = void>
        struct is_valid_monoid_ops : std::false_type {};

        template<typename O, typename W>
        struct is_valid_monoid_ops<O, W, std::void_t<
            decltype(O::identity()),
            decltype(O::combine(std::declval<W>(), std::declval<W>()))
        >> : std::bool_constant<
            std::is_convertible_v<decltype(O::identity()), W> &&
            std::is_convertible_v<
                decltype(O::combine(std::declval<W>(), std::declval<W>())), W
            >
        > {};

        template<typename O, typename W, typename = void>
        struct is_valid_monoid_selective_ops : std::false_type {};

        template<typename O, typename W>
        struct is_valid_monoid_selective_ops<O, W,
            std::enable_if_t<O::is_selective && is_valid_monoid_ops<O, W>::value>
        > : std::true_type {};

        template<typename O, typename W, typename = void>
        struct is_valid_idempotent_ops : std::false_type {};

        template<typename O, typename W>
        struct is_valid_idempotent_ops<O, W,
            std::enable_if_t<O::is_idemp && is_valid_monoid_ops<O, W>::value>
        > : std::true_type {};

        template<typename O, typename W, typename = void>
        struct is_valid_monoid_invertible_ops : std::false_type {};

        template<typename O, typename W>
        struct is_valid_monoid_invertible_ops<O, W,
            std::enable_if_t<O::is_invertible && is_valid_monoid_ops<O, W>::value>
        > : std::true_type {};

        template<typename O, typename W, typename = void>
        struct is_valid_range_add_compat_ops : std::false_type {};

        template<typename O, typename W>
        struct is_valid_range_add_compat_ops<O, W,
            std::enable_if_t<O::is_range_add_compat && is_valid_monoid_ops<O, W>::value>
        > : std::true_type {};
    }

    namespace cmpr {
        template<typename T>
        struct less {
            inline bool operator()(const T& a, const T& b) const { return a < b; }
        };

        template<typename T>
        struct greater {
            inline bool operator()(const T& a, const T& b) const { return a > b; }
        };

        template<typename T>
        struct equal {
            inline bool operator()(const T& a, const T& b) const { return a == b; }
        };

        template<typename T>
        struct less_equal {
            inline bool operator()(const T& a, const T& b) const { return a <= b; }
        };

        template<typename T>
        struct greater_equal {
            inline bool operator()(const T& a, const T& b) const { return a >= b; }
        };

        template<typename T>
        struct not_equal {
            inline bool operator()(const T& a, const T& b) const { return a != b; }
        };
    } // namespace dsa::utility
}

#endif // DSA_UTILITY_HPP
