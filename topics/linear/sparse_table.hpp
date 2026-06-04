#ifndef DSA_LINEAR_SPARSE_TABLE_HPP
#define DSA_LINEAR_SPARSE_TABLE_HPP

#include "utility.hpp"

#include <cmath>
#include <vector>

using namespace dsa::utility::monoid;

namespace dsa::linear::sparse_table {
    /**
     * @brief Static range-query structure supporting any associative monoid.
     *
     * Precomputes Ops::combine over every interval of length 2^j, then answers
     * range queries by decomposing [l,r] into O(log n) disjoint power-of-two blocks.
     *
     * @tparam T           Scalar type stored at each position.
     * @tparam OpsTemplate Monoid template (sumops, maxops, minops, …).
     *                     Instantiated as OpsTemplate<T> normally, or
     *                     OpsTemplate<IndexNode> when IndexReturn=true.
     * @tparam IndexReturn When true, query_with_index() also returns the 0-based
     *                     position of the winning element. Requires a comparative
     *                     monoid (maxops or minops).
     *
     * Build: O(n log n) time and space.  Query: O(log n).
     * For idempotent ops prefer IdempotentSparseTable (O(1) query).
     *
     * @code
     *   GenericSparseTable<int> st(arr);               // sumops, value only
     *   GenericSparseTable<int, maxops, true> st(arr); // max + index
     *   auto [idx, val] = st.query_with_index(l, r);
     * @endcode
     */
    template<
        typename T,
        template<typename> class OpsTemplate = sumops,
        bool IndexReturn = false
    >
    class GenericSparseTable {
        public:
            /** Leaf descriptor used when IndexReturn=true (0-based index + value). */
            struct IndexNode {
                int index{-1};
                T value{};
                static constexpr IndexNode lower_bound() { return {-1, std::numeric_limits<T>::lowest()}; }
                static constexpr IndexNode upper_bound() { return {-1, std::numeric_limits<T>::max()}; }
                bool operator<(const IndexNode& o) const { return value < o.value; }
                bool operator>(const IndexNode& o) const { return value > o.value; }
            };

        protected:
            /** Actual type stored in the table: T or IndexNode. */
            using NodeType = std::conditional_t<IndexReturn, IndexNode, T>;
            /** Fully resolved monoid operating on NodeType. */
            using Ops = std::conditional_t<IndexReturn, OpsTemplate<IndexNode>, OpsTemplate<T>>;

            /** sparse_table_[i][j] = Ops::combine of arr[i .. i + 2^j - 1] */
            std::vector<std::vector<NodeType>> sparse_table_;
            int n_;
            int num_levels_;   // floor(log2(n)) + 1

            /** Constructs a leaf NodeType from a 0-based index and scalar value. */
            static inline NodeType node(int index, T value) {
                if constexpr (IndexReturn) return {index, value};
                else return value;
            }

            /**
             * Fills sparse_table_ bottom-up: level 0 = raw elements,
             * level j = combine(left half, right half) of level j-1.
             */
            void build_sparse_table(const std::vector<T>& arr){
                sparse_table_.assign(n_, std::vector<NodeType>(num_levels_, Ops::identity()));
                for (int i = 0; i < n_; ++i) sparse_table_[i][0] = node(i, arr[i]);
                for (int j = 1; j < num_levels_; ++j)
                    for (int i = 0; i + (1 << j) - 1 < n_; ++i)
                        sparse_table_[i][j] = Ops::combine(sparse_table_[i][j-1], sparse_table_[i+(1<<(j-1))][j-1]);
            }

        public:
            static_assert(
                is_valid_monoid_ops<Ops, NodeType>::value,
                "OpsTemplate<NodeType> must have ::identity() -> NodeType and ::combine(NodeType, NodeType) -> NodeType"
            );
            static_assert(
                !IndexReturn || is_valid_monoid_selective_ops<Ops, NodeType>::value,
                "When IndexReturn=true, OpsTemplate must be a comparative monoid (maxops or minops)"
            );

            explicit GenericSparseTable(const std::vector<T>& arr)
                : n_(static_cast<int>(arr.size())),
                  num_levels_(static_cast<int>(std::floor(std::log2(n_))) + 1){
                build_sparse_table(arr);
            }

            /** Returns Ops::combine over arr[l..r] (0-based, inclusive). O(log n). */
            T query(int l, int r) const {
                NodeType target = Ops::identity();
                int len = r - l + 1;
                for (int j = 0; j < num_levels_; ++j){
                    if (1 & (len >> j)){
                        target = Ops::combine(target, sparse_table_[l][j]);
                        l = l + (1 << j);
                    }
                }
                if constexpr (IndexReturn) return target.value;
                else return target;
            }

            /**
             * @brief Returns the IndexNode (index + value) of the winning element
             *        over arr[l..r] (0-based, inclusive). Only valid when IndexReturn=true.
             */
            IndexNode query_with_index(int l, int r) const {
                static_assert(IndexReturn, "query_with_index is only available when IndexReturn=true");
                NodeType target = Ops::identity();
                int len = r - l + 1;
                for (int j = 0; j < num_levels_; ++j){
                    if (1 & (len >> j)){
                        target = Ops::combine(target, sparse_table_[l][j]);
                        l = l + (1 << j);
                    }
                }
                return target;
            }
    };

    /**
     * @brief Sparse table specialised for idempotent ops (max, min, gcd, and, or).
     *
     * Exploits idempotency — combine(x, x) == x — to answer queries with two
     * overlapping blocks in O(1). Inherits storage and build from GenericSparseTable.
     *
     * Build: O(n log n).  Query: O(1).
     *
     * @code
     *   IdempotentSparseTable<int> st(arr);         // maxops by default
     *   IdempotentSparseTable<int, minops> mn(arr);
     * @endcode
     */
    template<
        typename T,
        template<typename> class OpsTemplate = maxops
    >
    class IdempotentSparseTable : public GenericSparseTable<T, OpsTemplate, false> {
        public:
            static_assert(
                is_valid_idempotent_ops<OpsTemplate<T>, T>::value,
                "OpsTemplate must be an idempotent monoid (maxops, minops, gcdops, andops, orops)"
            );

            using GenericSparseTable<T, OpsTemplate, false>::GenericSparseTable;

            /**
             * Returns OpsTemplate::combine over arr[l..r] (0-based, inclusive).
             * Picks k = floor(log2(r-l+1)), merges two overlapping length-2^k blocks.
             * O(1).
             */
            T query(int l, int r) const {
                int k = static_cast<int>(std::floor(std::log2(r - l + 1)));
                return OpsTemplate<T>::combine(this->sparse_table_[l][k],
                                               this->sparse_table_[r - (1 << k) + 1][k]);
            }
    };
}

#endif // DSA_LINEAR_SPARSE_TABLE_HPP
