#ifndef DSA_TREE_FENWICK_TREE_HPP
#define DSA_TREE_FENWICK_TREE_HPP

#include <vector>

#include "utility.hpp"

using namespace dsa::utility;

namespace dsa::tree {
    /**
     * @brief Classic range-add / range-sum Fenwick tree (1-indexed).
     *
     * Supports:
     *   - assign(i, v):       set arr[i] = v.              O(log n).
     *   - add_range(l, r, d): add d to every arr[i] in [l,r]. O(log n).
     *   - prefix_sum(i):      sum of arr[1..i].            O(log n).
     *   - range_sum(l, r):    sum of arr[l..r].            O(log n).
     *
     * Uses the classic two-BIT trick (bit_a_, bit_b_) to support range-add
     * with prefix-sum queries in O(log n).
     */
    class SumFenwickTree {
        private:
            std::vector<int> bit_a_;
            std::vector<int> bit_b_;
            int n_;

            int sum(const std::vector<int>& bit, int index) const {
                int result = 0;
                while (index > 0) {
                    result += bit[index];
                    index -= index & -index;
                }
                return result;
            }

            void add(std::vector<int>& bit, int index, int delta) {
                while (index <= n_) {
                    bit[index] += delta;
                    index += index & -index;
                }
            }

        public:
            explicit SumFenwickTree(int n) : bit_a_(n + 2, 0), bit_b_(n + 2, 0), n_(n + 1) {}

            /** Returns the prefix sum on [1, index]. */
            int prefix_sum(int index) const {
                return sum(bit_a_, index) * index - sum(bit_b_, index);
            }

            /** Returns the range sum on [l, r]. */
            int range_sum(int l, int r) const {
                if (l > r) return 0;
                return prefix_sum(r) - prefix_sum(l - 1);
            }

            /** Adds delta to every arr[i] for i in [l, r]. */
            void add_range(int l, int r, int delta) {
                add(bit_a_, l, delta);
                add(bit_a_, r + 1, -delta);
                add(bit_b_, l, delta * (l - 1));
                add(bit_b_, r + 1, -delta * r);
            }

            /** Sets arr[index] = value. */
            void assign(int index, int value) {
                int current_value = range_sum(index, index);
                add_range(index, index, value - current_value);
            }
    };

    /**
     * @brief Generic Fenwick tree supporting prefix-query and point-update.
     *
     * @tparam WeighType   Scalar type stored at each position (e.g. int, float).
     * @tparam OpsTemplate Monoid template.
     *                     - IndexReturn=false: must be invertible (sumops).
     *                       Supports point-add and prefix/range-sum.
     *                     - IndexReturn=true:  must be selective (maxops/minops).
     *                       Supports point-update (monotone only) and prefix-max/min.
     * @tparam IndexReturn When true, query_with_index() returns the 0-based position
     *                     of the winning element alongside its value.
     *
     * Index convention: all public methods use 0-based indices [0, n-1].
     *
     * Complexity: O(n) build, O(log n) update and query.
     *
     * @note For IndexReturn=true (maxops/minops), point updates are monotone-only:
     *       update(i, v) is correct only when v >= arr[i] (max) or v <= arr[i] (min).
     *       Decreasing a maximum (or increasing a minimum) yields stale ancestors.
     *
     * Usage examples:
     * @code
     *   // Sum tree (point-add, prefix/range-sum)
     *   ModernFenwichTree<int> bit(arr);
     *   bit.update(2, 5);                 // arr[2] += 5
     *   int s = bit.range_query(1, 4);    // sum arr[1..4]
     *
     *   // Max tree with index tracking (monotone point-update, prefix-max)
     *   ModernFenwichTree<int, monoid::maxops, true> bit(arr);
     *   bit.update(3, 10);                // arr[3] = max(arr[3], 10)
     *   auto [idx, val] = bit.query_with_index(4);  // max of arr[0..4] with its index
     * @endcode
     */
    template<
        typename WeighType = float,
        template<typename> class OpsTemplate = monoid::sumops,
        bool IndexReturn = false
    >
    class ModernFenwichTree {
        public:
            /**
             * @brief Leaf descriptor used when IndexReturn=true.
             *
             * Carries the 0-based source index alongside its value so that
             * combine() can propagate the winning position up the tree.
             * lower_bound()/upper_bound() satisfy the identity requirements of
             * maxops<IndexNode> and minops<IndexNode> respectively.
             */
            struct IndexNode {
                int index{-1};
                WeighType value{};
                static constexpr IndexNode lower_bound() { return {-1, std::numeric_limits<WeighType>::lowest()}; }
                static constexpr IndexNode upper_bound() { return {-1, std::numeric_limits<WeighType>::max()}; }
                bool operator<(const IndexNode& o) const { return value < o.value; }
                bool operator>(const IndexNode& o) const { return value > o.value; }
                operator WeighType() const { return value; }
            };

        protected:
            /** Actual node type stored in the tree array. */
            using NodeType = std::conditional_t<IndexReturn, IndexNode, WeighType>;
            /** Fully resolved monoid: operates on NodeType. */
            using Ops = std::conditional_t<IndexReturn, OpsTemplate<IndexNode>, OpsTemplate<WeighType>>;

            /** Identity element used to initialise empty nodes. */
            static constexpr NodeType kNil = Ops::identity();

            /** Constructs a leaf NodeType from a 0-based index and scalar value. */
            static inline NodeType node(int index, WeighType value) {
                if constexpr (IndexReturn) return {index, value};
                else return value;
            }

        private:
            int n_;
            // bit_[i] (i in [1,n]) stores Ops::combine over arr[i - LSB(i) + 1 .. i]
            // where LSB(i) = i & -i (lowest set bit = range length covered by i).
            std::vector<NodeType> bit_;

        protected:
            /**
             * @brief O(n) bottom-up BIT construction.
             *
             * For each position i, first incorporate arr[i-1], then propagate
             * bit_[i] into bit_[i + LSB(i)] (the immediate ancestor).
             * Works correctly for both invertible (sum) and selective (max/min) monoids.
             */
            void build_fenwich(const std::vector<WeighType>& arr) {
                for (int i = 1; i <= n_; i++) {
                    bit_[i] = Ops::combine(bit_[i], node(i - 1, arr[i - 1]));
                    int parent = i + (i & -i);
                    if (parent <= n_) bit_[parent] = Ops::combine(bit_[parent], bit_[i]);
                }
            }

            /** Returns Ops::combine over arr[1..pos] (1-indexed). */
            NodeType _query(int pos) {
                NodeType q = kNil;
                while (pos > 0) {
                    q = Ops::combine(q, bit_[pos]);
                    pos -= pos & (-pos);
                }
                return q;
            }

            /**
             * @brief Propagates a node update upward through BIT ancestors.
             *
             * For sumops (IndexReturn=false): node(_, delta) = delta, so each
             *   ancestor accumulates += delta. Equivalent to point-add.
             * For maxops/minops (IndexReturn=true): node(i, val) = IndexNode{i, val},
             *   each ancestor takes Ops::combine (max or min). Correct only when
             *   the new value dominates the old (monotone update).
             */
            void _update(int pos, WeighType val) {
                NodeType nd = node(pos - 1, val);
                while (pos <= n_) {
                    bit_[pos] = Ops::combine(bit_[pos], nd);
                    pos += pos & (-pos);
                }
            }

        public:
            static_assert(
                (IndexReturn  && monoid::is_valid_monoid_selective_ops<Ops, NodeType>::value) ||
                (!IndexReturn && monoid::is_valid_monoid_invertible_ops<Ops, NodeType>::value),
                "OpsTemplate must be a selective monoid (maxops/minops) when IndexReturn=true, "
                "and an invertible monoid (sumops) when IndexReturn=false"
            );

            /** Constructs an empty tree of size n (all elements = identity). */
            explicit ModernFenwichTree(int n) : n_(n), bit_(n + 1, kNil) {}

            /** Constructs a tree initialised from arr (0-based). */
            explicit ModernFenwichTree(const std::vector<WeighType>& arr)
                : n_(arr.size()), bit_(arr.size() + 1, kNil) {
                build_fenwich(arr);
            }

            /**
             * @brief Point update at position i (0-based).
             *
             * For sumops:        adds val to arr[i].
             * For maxops/minops: takes Ops::combine(arr[i], val) — monotone only.
             */
            void update(int i, WeighType val) {
                _update(i + 1, val);
            }

            /** Returns Ops::combine over arr[0..i] (0-based, inclusive). */
            WeighType query(int i) {
                return _query(i + 1);
            }

            /**
             * @brief Returns Ops::combine over arr[l..r] (0-based, inclusive).
             * Only available when IndexReturn=false (invertible ops, i.e. sumops).
             */
            WeighType range_query(int l, int r) {
                static_assert(!IndexReturn,
                    "range_query is only available when IndexReturn=false (invertible ops)");
                return Ops::inverse(_query(r + 1), _query(l), kNil);
            }

            /**
             * @brief Returns the IndexNode (index + value) of the winning element
             *        over arr[0..i] (0-based, inclusive).
             * Only available when IndexReturn=true.
             */
            IndexNode query_with_index(int i) {
                static_assert(IndexReturn,
                    "query_with_index is only available when IndexReturn=true");
                return _query(i + 1);
            }
    };

}  // namespace dsa::tree

#endif  // DSA_TREE_FENWICK_TREE_HPP
