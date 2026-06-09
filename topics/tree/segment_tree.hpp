#ifndef DSA_TREE_SEGMENT_TREE_HPP
#define DSA_TREE_SEGMENT_TREE_HPP

#include <ostream>
#include <utility>
#include <vector>

#include "utility.hpp"

using namespace dsa::utility;

namespace dsa::tree {
    /**
     * @brief Support for two operations:
     *        1. Update a[i] = k in O(logN)
     *        2. Query information in [x, y] (information: min, max, sum) in O(logN)
     * @note: Index run from 1->N
     */
    class ClassicSegmentTree {
    private:
        struct Node {
            int l, r;
            int value{0};
        };

        std::vector<Node> nodes_;
        std::vector<int> leaves_;

        /**
         * @brief Builds segment tree nodes for interval `[l, r]`.
         */
        void build(int t, int l, int r) {
            nodes_[t].l = l;
            nodes_[t].r = r;

            if (l == r) {
                leaves_[l] = t;
                return;
            }

            int mid = (l + r) / 2;
            build(2 * t, l, mid);
            build(2 * t + 1, mid + 1, r);
        }

        /**
         * @brief Queries `[l, r]` while currently at tree node `t`.
         */
        int query(int t, int l, int r) {
            if (l > r || r < nodes_[t].l || nodes_[t].r < l) return 0;
            if (l <= nodes_[t].l && nodes_[t].r <= r) return nodes_[t].value;
            return query(2 * t, l, r) + query(2 * t + 1, l, r);
        }

    public:
        explicit ClassicSegmentTree(int n) : nodes_(4 * n + 5), leaves_(n + 1) {
            build(1, 1, n);
        }

        void update(int index, int value) {
            int t = leaves_[index];
            nodes_[t].value = value;
            while (t != 1) {
                t /= 2;
                nodes_[t].value = nodes_[2 * t].value + nodes_[2 * t + 1].value;
            }
        }

        int query(int l, int r) {
            return query(1, l, r);
        }
    };

    /**
     * @brief Generic segment tree supporting point-update and range-query.
     *
     * @tparam WeightType  Scalar type stored at each position (e.g. int, float).
     * @tparam OpsTemplate Monoid template (sumops, maxops, minops, …).
     *                     Instantiated as OpsTemplate<WeightType> normally, or
     *                     OpsTemplate<IndexNode> when IndexReturn=true.
     * @tparam IndexReturn When true, query_with_index() returns the position of
     *                     the winning element alongside its value.
     *                     Requires OpsTemplate to be a comparative monoid
     *                     (maxops or minops).
     *
     * Index convention: all public methods use 0-based indices [0, n-1].
     *
     * Complexity: O(n) build, O(log n) update and query.
     *
     * Usage examples:
     * @code
     *   // Sum tree
     *   ModernIndexBasedSegmentTree<int> seg(arr);
     *   seg.update(2, 10);
     *   int s = seg.query(0, 4);
     *
     *   // Max tree that also returns the index of the maximum
     *   ModernIndexBasedSegmentTree<int, monoid::maxops, true> seg(arr);
     *   auto [idx, val] = seg.query_with_index(l, r);
     * @endcode
     */
    template<
        typename WeightType = float,
        template<typename> class OpsTemplate = monoid::sumops,
        bool IndexReturn = false
    >
    class ModernIndexBasedSegmentTree {
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
                WeightType value{};
                static constexpr IndexNode lower_bound() { return {-1, std::numeric_limits<WeightType>::lowest()}; }
                static constexpr IndexNode upper_bound() { return {-1, std::numeric_limits<WeightType>::max()}; }
                bool operator<(const IndexNode& o) const { return value < o.value; }
                bool operator>(const IndexNode& o) const { return value > o.value; }
                operator WeightType() const { return value; }
            };

        protected:
            /** Actual node type stored in the tree array. */
            using NodeType = std::conditional_t<IndexReturn, IndexNode, WeightType>;
            /** Fully resolved monoid: operates on NodeType. */
            using Ops = std::conditional_t<IndexReturn, OpsTemplate<IndexNode>, OpsTemplate<WeightType>>;

            /** Identity element, used to initialise empty nodes. */
            static constexpr NodeType kNil = Ops::identity();

            /** Constructs a leaf NodeType from a 0-based index and scalar value. */
            static inline NodeType node(int index, WeightType value){
                if constexpr (IndexReturn){
                    return {index, value};
                }
                else {
                    return value;
                }
            }

        private:
            int n_;
            std::vector<NodeType> tree_;

        protected:
            /** Initialises all nodes to kNil (empty tree). */
            void build(int t, int l, int r){
                if (l == r){
                    tree_[t] = kNil;
                    return;
                }
                int m = (l + r) / 2;
                build(2 * t, l, m);
                build(2 * t + 1, m + 1, r);
                tree_[t] = kNil;
            }

            /** Builds the tree bottom-up from arr (1-indexed internally). */
            void build(int t, int l, int r, const std::vector<WeightType>& arr){
                if (l == r){
                    tree_[t] = node(l-1, arr[l-1]);
                    return;
                }
                int m = (l + r) / 2;
                build(2 * t, l, m, arr);
                build(2 * t + 1, m + 1, r, arr);
                tree_[t] = Ops::combine(tree_[2 * t], tree_[2 * t + 1]);
            }

            /** Returns Ops::combine over [a,b] while traversing node t covering [l,r]. */
            NodeType query(int t, int l, int r, int a, int b){
                if (a <= l && r <= b) return tree_[t];
                if (l > r || b < l || r < a) return Ops::identity();
                int m = (l + r) / 2;
                return Ops::combine(query(2 * t, l, m, a, b), query(2 * t + 1, m + 1, r, a, b));
            }

            /** Sets position pos (1-based) to k and pulls up ancestor nodes. */
            void update(int t, int l, int r, int pos, WeightType k){
                if (l == r){
                    tree_[t] = node(l-1, k);
                    return;
                }
                int m = (l + r) / 2;
                if (pos <= m){
                    update(2 * t, l, m, pos, k);
                }
                else {
                    update(2 * t + 1, m + 1, r, pos, k);
                }
                tree_[t] = Ops::combine(tree_[2 * t], tree_[2 * t + 1]);
            }

        public:
            static_assert(
                monoid::is_valid_monoid_ops<Ops, NodeType>::value,
                "OpsTemplate<NodeType> must have ::identity() -> NodeType and ::combine(NodeType, NodeType) -> NodeType"
            );

            static_assert(
                !IndexReturn || monoid::is_valid_monoid_selective_ops<Ops, NodeType>::value,
                "When IndexReturn=true, OpsTemplate must be a comparative monoid (maxops or minops)"
            );

        public:
            /** Constructs an empty tree of size n. */
            explicit ModernIndexBasedSegmentTree(int n) : n_(n), tree_(4 * n){
                build(1, 1, n_);
            }

            /** Constructs a tree initialised from arr (0-based). */
            explicit ModernIndexBasedSegmentTree(const std::vector<WeightType>& arr) : n_(arr.size()), tree_(4 * n_){
                build(1, 1, n_, arr);
            }

            /** Returns Ops::combine over arr[l..r] (0-based, inclusive). */
            WeightType query(int l, int r){
                return query(1, 1, n_, l+1, r+1);
            }

            /** Sets arr[i] = v (0-based) in O(log n). */
            void update(int i, WeightType v){
                update(1, 1, n_, i+1, v);
            }

            /**
             * @brief Returns the IndexNode (index + value) of the winning element
             *        over arr[l..r] (0-based, inclusive).
             * Only available when IndexReturn=true.
             */
            IndexNode query_with_index(int l, int r){
                static_assert(IndexReturn, "query_with_index is only available when IndexReturn=true");
                return query(1, 1, n_, l+1, r+1);
            }
    };

    /**
     * @brief Generic segment tree with range-add lazy propagation.
     *
     * Supports:
     *   - update(l, r, delta): add delta to all arr[i] for i in [l, r]. O(log n).
     *   - update(i, v):        point-set arr[i] = v.                     O(log n).
     *   - query(l, r):         aggregate over arr[l..r].                 O(log n).
     *
     * @tparam WeightType  Scalar type (must support +, *, numeric operations).
     * @tparam OpsTemplate Monoid template. Only sum/max/min are compatible with
     *                     range-add lazy (additive delta must shift aggregate
     *                     linearly). Other ops (gcd, or, and) are rejected at
     *                     compile time.
     * @tparam IndexReturn When true, query_with_index() returns the 0-based
     *                     position of the winning element. Requires maxops or minops.
     *
     * LazyType is always WeightType (additive delta) — no extra struct needed.
     * compose: new_delta + old_delta.
     * apply:   idempotent ops (max/min) → val + delta;
     *          invertible ops (sum)      → val + delta * len.
     *
     * @code
     *   ModernLazySegmentTree<int> seg(arr);           // sum + range-add
     *   seg.update(1, 4, 3);                           // arr[1..4] += 3
     *   seg.query(0, 5);
     *
     *   ModernLazySegmentTree<int, monoid::maxops, true> seg(arr);
     *   seg.update(0, 3, -1);
     *   auto [idx, val] = seg.query_with_index(0, 5);
     * @endcode
     */
    template<
        typename WeightType,
        template<typename> class OpsTemplate = monoid::sumops,
        bool IndexReturn = false
    >
    class ModernLazySegmentTree {
        public:
            /** Leaf descriptor used when IndexReturn=true (0-based index + value). */
            struct IndexNode {
                int index{-1};
                WeightType value{};
                static constexpr IndexNode lower_bound() { return {-1, std::numeric_limits<WeightType>::lowest()}; }
                static constexpr IndexNode upper_bound() { return {-1, std::numeric_limits<WeightType>::max()}; }
                bool operator<(const IndexNode& o) const { return value < o.value; }
                bool operator>(const IndexNode& o) const { return value > o.value; }
                IndexNode operator+(const WeightType& o) const { return {index, value + o}; }
            };

        protected:
            /** Actual type stored per tree node: WeightType or IndexNode. */
            using NodeType = std::conditional_t<IndexReturn, IndexNode, WeightType>;
            /** Monoid operating on NodeType for combine/identity. */
            using Ops      = std::conditional_t<IndexReturn, OpsTemplate<IndexNode>, OpsTemplate<WeightType>>;
            /** Lazy delta is always a scalar WeightType (additive range-add). */
            using LazyType = WeightType;

            struct TreeNode {
                NodeType val  = Ops::identity();
                LazyType lazy = LazyType(0);
            };

            static constexpr NodeType kNil     = Ops::identity();
            static constexpr LazyType kLazyNil = LazyType(0);

            /** Constructs a leaf NodeType from 0-based index and scalar value. */
            static inline NodeType node(int index, WeightType value) {
                if constexpr (IndexReturn) return {index, value};
                else return value;
            }

            /**
             * @brief Applies additive delta to a WeightType aggregate.
             *        max/min (idempotent): aggregate shifts by delta (len irrelevant).
             *        sum     (invertible): aggregate grows by delta * len.
             */
            static WeightType apply_to_weight(WeightType val, LazyType delta, int len) {
                if constexpr (monoid::is_valid_monoid_selective_ops<OpsTemplate<WeightType>, WeightType>::value)
                    return val + delta;
                else
                    return val + delta * WeightType(len);
            }

            /**
             * @brief Applies delta to a NodeType.
             *        When IndexReturn=true, only .value is updated; .index is preserved.
             */
            static NodeType apply_lazy(const NodeType& nd, LazyType delta, int len) {
                if constexpr (IndexReturn)
                    return {nd.index, apply_to_weight(nd.value, delta, len)};
                else
                    return apply_to_weight(nd, delta, len);
            }

            /** Stacks new lazy on top of existing (additive composition). */
            static LazyType compose_lazy(LazyType new_delta, LazyType old_delta) {
                return new_delta + old_delta;
            }

        private:
            int n_;
            std::vector<TreeNode> tree_;

        protected:
            /** Builds tree bottom-up from arr (1-indexed internally). */
            void build(int t, int l, int r, const std::vector<WeightType>& arr){
                if (l == r){
                    tree_[t] = {node(l-1, arr[l-1]), kLazyNil};
                    return;
                }
                int m = (l+r)/2;
                build(2*t,l,m,arr);
                build(2*t+1,m+1,r,arr);
                tree_[t].val = Ops::combine(tree_[2*t].val, tree_[2*t+1].val);
            }

            /** Pushes tree_[t].lazy down to children t*2 and t*2+1, then clears it. */
            void push_down(int t, int l, int r){
                if (l > r || tree_[t].lazy == kLazyNil) return;
                LazyType lazy = tree_[t].lazy;
                int m = (l + r) / 2;
                // update real value for children
                tree_[2*t].val = apply_lazy(tree_[2*t].val, lazy, m-l+1);
                tree_[2*t+1].val = apply_lazy(tree_[2*t+1].val, lazy, r-m);
                // transfer lazy debt to children
                tree_[2*t].lazy = compose_lazy(tree_[2*t].lazy, lazy);
                tree_[2*t+1].lazy = compose_lazy(tree_[2*t+1].lazy, lazy);

                // reset lazy debt in current node
                tree_[t].lazy = kLazyNil;
            }

            /** Returns aggregate over [a,b] while traversing node t covering [l,r]. */
            NodeType query(int t, int l, int r, int a, int b){
                // case 1: [l...r] [a...b] or [a...b] [l...r]
                if (l > r || r < a || b < l) return kNil;
                // case 2: [a...l...r...b]
                if (a <= l && r <= b) return tree_[t].val;
                // case 3: overlap, fallback to children
                // transfer lazy debt into children
                push_down(t, l, r);
                // get the query
                int m = (l+r)/2;
                return Ops::combine(query(2*t,l,m,a,b), query(2*t+1,m+1,r,a,b));
            }

            /** Sets position pos (0-based) to k (point update, clears lazy path). */
            void update(int t, int l, int r, int pos, WeightType k){
                if (l == r){
                    tree_[t] = {node(l-1, k), kLazyNil};
                    return;
                }
                push_down(t, l, r);
                int m = (l+r)/2;
                (pos <= m) ? update(2*t,l,m,pos,k) : update(2*t+1,m+1,r,pos,k);
                tree_[t].val = Ops::combine(tree_[2*t].val, tree_[2*t+1].val);
            }

            void update(int t, int l, int r, int a, int b, WeightType k){
                // case 1: [l...r] [a...b] or [a...b] [l...r]
                if (l > r || r < a || b < l) return;
                // case 2.1: t is leaf node
                if (l == r){
                    tree_[t] = {tree_[t].val + k, kLazyNil};
                    return;
                }
                // case 2.2: [a...l...r...b]
                if (a <= l && r <= b){
                    tree_[t].val = apply_lazy(tree_[t].val, k, r-l+1);
                    tree_[t].lazy = compose_lazy(tree_[t].lazy, k);
                    return;
                }
                // case 3: overlap, fallback to children
                push_down(t, l, r);
                int m = (l+r) / 2;
                update(2*t,l,m,a,b,k);
                update(2*t+1,m+1,r,a,b,k);
                tree_[t].val = Ops::combine(tree_[2*t].val, tree_[2*t+1].val);
            }

        public:
            static_assert(
                monoid::is_valid_range_add_compat_ops<OpsTemplate<WeightType>, WeightType>::value,
                "OpsTemplate must be range-add compatible: only sumops / maxops / minops are supported"
            );
            static_assert(
                !IndexReturn || monoid::is_valid_monoid_selective_ops<Ops, NodeType>::value,
                "When IndexReturn=true, OpsTemplate must be a comparative monoid (maxops or minops)"
            );

        public:
            /** Constructs an empty tree of size n (all elements = identity). */
            explicit ModernLazySegmentTree(int n) : n_(n), tree_(4 * n) {}

            /** Constructs a tree initialised from arr (0-based). */
            explicit ModernLazySegmentTree(const std::vector<WeightType>& arr)
                : n_(arr.size()), tree_(4 * n_) {
                build(1, 1, n_, arr);
            }

            /** Returns Ops::combine over arr[l..r] (0-based, inclusive). O(log n). */
            WeightType query(int l, int r) {
                return query(1, 1, n_, l+1, r+1);
            }

            /** Adds delta to every arr[i] for i in [l, r] (0-based). O(log n). */
            void update(int l, int r, WeightType delta) {
                update(1, 1, n_, l+1, r+1, delta);
            }

            /** Sets arr[i] = v (0-based). O(log n). */
            void update(int i, WeightType v) {
                update(1, 1, n_, i+1, v);
            }

            /**
             * @brief Returns the IndexNode (index + value) of the winning element
             *        over arr[l..r] (0-based, inclusive). Only valid when IndexReturn=true.
             */
            IndexNode query_with_index(int l, int r) {
                static_assert(IndexReturn, "query_with_index is only available when IndexReturn=true");
                return query(1, 1, n_, l+1, r+1);
            }
    };

/**
 * @brief Segment tree with lazy range-add propagation.
 *
 * Supports point assignment, range add, and range sum query over 1-indexed
 * positions.
 */
class LazySegmentTree {
private:
    struct Node {
        int l, r;
        int value{0};
        int lazy{0};
    };

    std::vector<Node> nodes_;
    std::vector<int> leaves_;
    int n_{0};

    void build(int t, int l, int r) {
        nodes_[t].l = l;
        nodes_[t].r = r;

        if (l == r) {
            leaves_[l] = t;
            return;
        }

        int mid = (l + r) / 2;
        build(2 * t, l, mid);
        build(2 * t + 1, mid + 1, r);
    }

    /**
     * @brief Pushes the pending lazy delta at node `t` to its children.
     */
    void push_down(int t) {
        int lazy = nodes_[t].lazy;
        if (lazy == 0) return;

        nodes_[t].lazy = 0;
        for (int child : {2 * t, 2 * t + 1}) {
            nodes_[child].lazy += lazy;
            nodes_[child].value += lazy * (nodes_[child].r - nodes_[child].l + 1);
        }
    }

    int query(int t, int l, int r) {
        if (l > r || r < nodes_[t].l || nodes_[t].r < l) return 0;
        if (l <= nodes_[t].l && nodes_[t].r <= r) return nodes_[t].value;

        push_down(t);
        return query(2 * t, l, r) + query(2 * t + 1, l, r);
    }

public:
    LazySegmentTree() = default;

    explicit LazySegmentTree(int n) : nodes_(4 * n + 5), leaves_(n + 1), n_(n) {
        build(1, 1, n);
    }

    void update(int t, int index, int value) {
        int l = nodes_[t].l;
        int r = nodes_[t].r;
        if (index < l || r < index) return;

        if (l == r) {
            nodes_[t].value = value;
            return;
        }

        push_down(t);
        update(2 * t, index, value);
        update(2 * t + 1, index, value);
        nodes_[t].value = nodes_[2 * t].value + nodes_[2 * t + 1].value;
    }

    void update(int t, int ql, int qr, int delta) {
        int l = nodes_[t].l;
        int r = nodes_[t].r;
        if (r < ql || qr < l) return;

        if (ql <= l && r <= qr) {
            nodes_[t].value += delta * (r - l + 1);
            nodes_[t].lazy += delta;
            return;
        }

        push_down(t);
        update(2 * t, ql, qr, delta);
        update(2 * t + 1, ql, qr, delta);
        nodes_[t].value = nodes_[2 * t].value + nodes_[2 * t + 1].value;
    }

    int query(int l, int r) {
        return query(1, l, r);
    }

    friend std::ostream& operator<<(std::ostream& os, const LazySegmentTree& tree) {
        os << "tree: \n";
        for (int id = 1; id <= tree.leaves_[tree.n_]; ++id) {
            os << "\t- [" << tree.nodes_[id].l << ", " << tree.nodes_[id].r << "]\t("
               << id << "): value = " << tree.nodes_[id].value
               << ", lazy = " << tree.nodes_[id].lazy << "\n";
        }
        os << "\n";
        return os;
    }
};

}  // namespace dsa::tree

#endif  // DSA_TREE_SEGMENT_TREE_HPP
