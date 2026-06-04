#ifndef DSA_LINEAR_SQRT_DECOMP_HPP
#define DSA_LINEAR_SQRT_DECOMP_HPP

#include "../utility.hpp"

#include <vector>
#include <cmath>
#include <algorithm>

using namespace dsa::utility::monoid;

namespace dsa::linear::sqrt_decomp {

    /**
     * GenericSqrtDecomposition<T, Ops>
     *
     * Partitions arr into blocks of size ⌈√n⌉, precomputing Ops::combine per block.
     *
     * Build:              O(n)
     * query(l, r):        O(√n) — range aggregate [l, r]
     * update(k, val):     O(√n) — point set arr[k] = val, rebuild block
     *
     * Ops must satisfy is_valid_monoid_ops<Ops, T>:
     *   static T identity()
     *   static T combine(T a, T b)
     *
     * Example:
     *   std::vector<int> a{5, 2, 8, 1, 9, 3};
     *   GenericSqrtDecomposition<int> sd(a);  // default sumops
     *   sd.query(1, 4);                       // 2+8+1+9 = 20
     *   sd.update(2, 0);
     *   sd.query(1, 4);                       // 2+0+1+9 = 12
     */
    template<
        typename T,
        template<typename> class OpsTemplate = sumops
    >
    class GenericSqrtDecomposition {
        using Ops = OpsTemplate<T>;

        static_assert(
            is_valid_monoid_ops<Ops, T>::value,
            "OpsTemplate<T> must have ::identity() -> T and ::combine(T, T) -> T"
        );

    public:
        GenericSqrtDecomposition() = default;

        explicit GenericSqrtDecomposition(const std::vector<T>& arr) { build(arr); }

        void build(const std::vector<T>& arr) {
            arr_      = arr;
            n_        = arr.size();
            bsz_      = std::max<size_t>(1, (size_t)std::ceil(std::sqrt((double)n_)));
            n_blocks_ = (n_ + bsz_ - 1) / bsz_;
            blocks_.assign(n_blocks_, Ops::identity());
            for (size_t i = 0; i < n_; ++i)
                blocks_[i / bsz_] = Ops::combine(blocks_[i / bsz_], arr_[i]);
        }

        // O(√n): left partial + full middle blocks + right partial
        T query(int l, int r) const {
            int bl = l / (int)bsz_, br = r / (int)bsz_;
            T res = Ops::identity();
            if (bl == br) {
                for (int i = l; i <= r; ++i)
                    res = Ops::combine(res, arr_[i]);
                return res;
            }
            for (int i = l, e = (int)((bl + 1) * bsz_); i < e; ++i)
                res = Ops::combine(res, arr_[i]);
            for (int b = bl + 1; b < br; ++b)
                res = Ops::combine(res, blocks_[b]);
            for (int i = (int)(br * bsz_); i <= r; ++i)
                res = Ops::combine(res, arr_[i]);
            return res;
        }

        // O(1) if Ops::is_invertible (e.g. sumops), O(√n) otherwise (full rebuild)
        void update(int k, const T& val) {
            size_t b = k / bsz_;
            if constexpr (is_valid_monoid_invertible_ops<Ops, T>::value) {
                blocks_[b] = Ops::inverse(blocks_[b], arr_[k], val);
                arr_[k] = val;
            } else {
                arr_[k] = val;
                blocks_[b] = Ops::identity();
                for (size_t i = b * bsz_, e = std::min(i + bsz_, n_); i < e; ++i)
                    blocks_[b] = Ops::combine(blocks_[b], arr_[i]);
            }
        }

        size_t size() const { return n_; }

    private:
        std::vector<T>      arr_;
        std::vector<T>      blocks_;
        size_t              n_{}, bsz_{}, n_blocks_{};
    };

} // namespace dsa::linear::sqrt_decomp

#endif // DSA_LINEAR_SQRT_DECOMP_HPP
