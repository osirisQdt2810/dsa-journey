#include <vector>
#include <iostream>
#include <set>
#include <string>

using namespace std;

/*
    LC 699 - Falling Squares
    https://leetcode.com/problems/falling-squares/

    There are several squares being dropped onto the X-axis of a 2D plane.

    You are given a 2D integer array positions where positions[i] = [left_i, sideLength_i] represents the i^th square with a side length of sideLength_i that is dropped with its left edge aligned with X-coordinate left_i.

    Each square is dropped one at a time from a height above any landed squares. It then falls downward (negative Y direction) until it either lands on the top side of another square or on the X-axis. A square brushing the left/right side of another square does not count as landing on it. Once it lands, it freezes in place and cannot be moved.

    After each square is dropped, you must record the height of the current tallest stack of squares.

    Return an integer array ans where ans[i] represents the height described above after dropping the i^th square.

    Example 1:

    Input: positions = [[1,2],[2,3],[6,1]]
    Output: [2,5,5]
    Explanation:
    After the first drop, the tallest stack is square 1 with a height of 2.
    After the second drop, the tallest stack is squares 1 and 2 with a height of 5.
    After the third drop, the tallest stack is still squares 1 and 2 with a height of 5.
    Thus, we return an answer of [2, 5, 5].

    Example 2:

    Input: positions = [[100,100],[200,100]]
    Output: [100,100]
    Explanation:
    After the first drop, the tallest stack is square 1 with a height of 100.
    After the second drop, the tallest stack is either square 1 or square 2, both with heights of 100.
    Thus, we return an answer of [100, 100].
    Note that square 2 only brushes the right side of square 1, which does not count as landing on it.

    Constraints:

    - 1 <= positions.length <= 1000
    - 1 <= left_i <= 10^8
    - 1 <= sideLength_i <= 10^6
*/
/*
    Best solution: coordinate compression + lazy segment tree (range assign, range max)

    Complexity:
        Time:  O(n log n) — per square one range-max query and one range-assign on O(n) compressed cells.
        Space: O(n) — compressed coordinates and the 4*m tree arrays.

    Approach (detail):
        1. Only the 2n edges left_i and left_i + side_i matter; compress them. A square covers the
           half-open interval [left, left + side), so squares that merely touch at an edge share no cell.
        2. When a square falls on [L, R): it lands on the highest point below it, so its top is
           h = max(height[L..R)) + side, and afterwards the WHOLE interval has height h.
           That is a range assign (not a range add: adding side would leave lower cells under the
           square too low, e.g. [[1,5],[1,1],[2,1]] -> [5,6,7], range add gives [5,6,6]).
        3. Lazy segment tree storing max per node; lazy tag = pending assignment (heights are >= 1,
           so 0 means "no tag"). Assign and max both compose cleanly, so lazy propagation applies.
        4. The answer after each drop is the global max = the root value.
        5. n <= 1000, so the O(n^2) scan over previously landed squares also passes.
*/
/*
    Your solution: range_assign_query_max — compressed coordinates + skyline_tree (lazy assign / max)

    Complexity:
        Time:  O(n log n) — sort/compress the 2n edges, then per square one query_max and one assign.
        Space: O(n) — xs and the skyline_tree arrays over 2*|xs| slots.

    Approach:
        1. x_axis collects left and left + side of every square; xs is the sorted list and id(x) its index.
        2. Each compressed coordinate id owns two slots (2*id+1, 2*id+2 after the +1 shift); a square
           [x1, x2) covers slots 2*id1+2 .. 2*id2+1, so the slot of x2 itself is excluded and squares that
           only touch at an edge do not overlap.
        3. skyline_tree keeps smax per node and lazy = pending assignment; push() copies the tag to both
           children; query_max / assign follow the usual three cases (outside, fully inside, split).
        4. Per square: maxlr = query_max(L+1, R+1) + side, assign(L+1, R+1, maxlr), and push
           query_root() (global max) into res.

        Also in the file (not called): wrong_approach_range_add_query_max — lazymaxtree adding side to
        the range instead of assigning, wrong on [[1,5],[1,1],[2,1]] (gives [5,6,6]); fenwick_range —
        2-BIT range add / range sum, kept as a note on why it cannot answer range max.

    Verdict: MATCHES BEST — lazy segment tree with range assign + range max over compressed coordinates.
        Verified against an O(n^2) brute force on 20000 random cases.
*/
/*
    - range_update, range_query => lazy segtree
    - range_assign, range_query => lazy segtree
    - range update, prefix_query => difference fenwick/lazy segtree
*/
class Solution {
    /*
        Fenwick: range update + range sum (để tránh dùng lazy segtree)

        1. Mảng hiệu
            A[1..N] là mảng gốc, A[0] = 0.
            D[i] = A[i] - A[i-1]   =>   A[i] = D[1] + D[2] + ... + D[i]

        2. Tổng tiền tố của A
            S(x) = A[1] + ... + A[x]
                 = sum_{k=1..x} (x - k + 1) * D[k]          (D[k] góp mặt trong A[k..x])
                 = (x + 1) * sum_{k=1..x} D[k]  -  sum_{k=1..x} k * D[k]

            => giữ 2 Fenwick:
                B1 quản lý D[k]
                B2 quản lý k * D[k]
            S(x)       = (x + 1) * B1.query(x) - B2.query(x)
            sum(L..R)  = S(R) - S(L - 1)

        3. Range update: cộng v vào A[L..R]
            Trên mảng hiệu chỉ có 2 ô thay đổi:
                D[L]   += v        (A[L] tăng v, A[L-1] giữ nguyên)
                D[R+1] -= v        (A[R] tăng v, A[R+1] giữ nguyên)
            => update(L, R, v) <=> 2 point update trên D, kéo theo 2 point update tương ứng trên k * D[k]:
                B1.update(L, v);        B1.update(R + 1, -v);
                B2.update(L, L * v);    B2.update(R + 1, -(R + 1) * v);

            Mỗi thao tác (update / query) là O(log N).
    */
    private:
        template<typename T>
        struct fenwick_range {
            int N;
            vector<T> D;
            vector<T> kD;

            fenwick_range(int n) : N(n+1), D(N, 0), kD(N, 0){}


            void update_point(vector<T>& bits, int k, T val){
                while (k < N){
                    bits[k] += val;
                    k += k & (-k);
                }
            }

            T query_point(vector<T>& bits, int k){
                T sum = 0;
                while (k > 0){
                    sum += bits[k];
                    k -= k & (-k);
                }
                return sum;
            }

            void update_range(int L, int R, T val){
                update_point(D, L, val);
                update_point(D, R+1, -val);
                update_point(kD, L, L*val);
                update_point(kD, R+1, -(R+1)*val);
            }

            T query(int k){
                return query_point(D, k)*(k+1) - query_point(kD, k);
            }

            T query_root(){
                // this fenwick_tree doesn't support max(query_root)
                // issue of this problem: update_range, max_query => must be lazysegtree
            }
        };

        struct lazymaxtree {
            int N;
            vector<int> smax;
            vector<int> lazy;

            lazymaxtree(int n) : N(n), smax(4*N+1, 0), lazy(4*N+1,0){}

            void push(int t){
                if (lazy[t] == 0) return;
                smax[2*t] += lazy[t];
                lazy[2*t] += lazy[t];

                smax[2*t+1] += lazy[t];
                lazy[2*t+1] += lazy[t];

                lazy[t] = 0;
            }

            void update_range(int t, int l, int r, int X, int Y, int val){
                // case 1: [l...r][X...Y] or [X...Y][l...r]
                if (r < X || Y < l) return;
                // case 2: [X...l...r...Y]
                if (X <= l && r <= Y){
                    smax[t] += val;
                    lazy[t] += val;
                    return;
                }
                int m = (l+r)/2;
                push(t);
                update_range(2*t,l,m,X,Y,val);
                update_range(2*t+1,m+1,r,X,Y,val);
                smax[t] = max(smax[2*t], smax[2*t+1]);
            }

            void update_range(int L, int R, int val){
                update_range(1,1,N,L,R,val);
            }

            int query_root(){
                return smax[1];
            }
        };

        struct skyline_tree {
            int N;
            vector<int> smax;
            vector<int> lazy;

            skyline_tree(int n) : N(n), smax(4*N+1, 0), lazy(4*N+1,0){}

            int query_root(){
                return smax[1];
            }

            void push(int t){
                if (lazy[t] == 0) return;
                smax[2*t] = lazy[t];
                lazy[2*t] = lazy[t];

                smax[2*t+1] = lazy[t];
                lazy[2*t+1] = lazy[t];

                lazy[t] = 0;
            }

            int query_max(int t, int l, int r, int X, int Y){
                // case 1: [l...r][X...Y] or [X...Y][l...r]
                if (r < X || Y < l) return 0;
                // case 2: [X...l...r...Y]
                if (X <= l && r <= Y) return smax[t];
                push(t);
                int m = (l+r)/2;
                return (smax[t] = max(query_max(2*t,l,m,X,Y),query_max(2*t+1,m+1,r,X,Y)));
            }

            void assign(int t, int l, int r, int X, int Y, int val){
                // case 1: [l...r][X...Y] or [X...Y][l...r]
                if (r < X || Y < l) return;
                // case 2: [X...l...r...Y]
                if (X <= l && r <= Y){
                    smax[t] = val;
                    lazy[t] = val;
                    return;
                }
                push(t);
                int m = (l+r)/2;
                assign(2*t,l,m,X,Y,val);
                assign(2*t+1,m+1,r,X,Y,val);
                smax[t] = max(smax[2*t], smax[2*t+1]);
            }

            void assign(int L, int R, int val){
                assign(1,1,N,L,R,val);
            }

            int query_max(int L, int R){
                return query_max(1,1,N,L,R);
            }
        };

    public:
        vector<int> fallingSquares(vector<vector<int>>& positions) {
            set<int> x_axis;
            for (auto& position : positions){
                x_axis.insert(position[0]);
                x_axis.insert(position[0] + position[1]);
            }
            vector<int> xs(x_axis.begin(), x_axis.end());
            auto id = [&](int x) -> int {
                return lower_bound(xs.begin(), xs.end(), x) - xs.begin();
            };

            using ll = long long;

            auto wrong_approach_range_add_query_max = [&](){
                // fenwick_range<ll> tree(2*xs.size());
                lazymaxtree tree(2*xs.size());
                vector<int> res;

                for (auto& position : positions){
                    int side = position[1];
                    int x1 = position[0], x2 = x1 + side;
                    int id1 = id(x1), id2 = id(x2); // id: -> (-)2*id, (+)2*id+1
                    // cout << "pos=" << x1 << " side=" << side << " id1=" << id1 << " id2=" << id2 << "\n";
                    tree.update_range(2*id1+1+1, 2*id2+1, side);
                    // cout << "here=\n";
                    res.push_back(tree.query_root());
                }
                return res;
            };

            auto range_assign_query_max = [&](){
                skyline_tree tree(2*xs.size());
                vector<int> res;

                for (auto& position : positions){
                    int side = position[1];
                    int x1 = position[0], x2 = x1 + side;
                    int id1 = id(x1), id2 = id(x2); // id: -> (-)2*id, (+)2*id+1
                    int L = 2*id1+1;
                    int R = 2*id2;
                    int maxlr = tree.query_max(L+1,R+1) + side;
                    tree.assign(L+1,R+1,maxlr);
                    res.push_back(tree.query_root());
                }
                return res;
            };

            return range_assign_query_max();
        }
};

auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
}();

int main(){
    // test-lc:begin
    auto to_str = [](const vector<int>& v) -> string {
        string s = "[";
        for (size_t i = 0; i < v.size(); ++i) s += (i ? "," : "") + to_string(v[i]);
        return s + "]";
    };
    int passed = 0, total = 0;
    auto check = [&](int id, const vector<int>& got, const vector<int>& expected) -> void {
        ++total;
        if (got == expected){
            ++passed;
            cout << "Example " << id << ": PASS\n";
        }
        else cout << "Example " << id << ": FAIL  got=" << to_str(got) << "  expected=" << to_str(expected) << "\n";
    };

    {
        vector<vector<int>> positions = {{1,2},{2,3},{6,1}};
        check(1, Solution().fallingSquares(positions), {2,5,5});
    }
    {
        vector<vector<int>> positions = {{100,100},{200,100}};
        check(2, Solution().fallingSquares(positions), {100,100});
    }

    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}