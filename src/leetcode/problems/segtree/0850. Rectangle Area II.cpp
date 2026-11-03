#include <vector>
#include <iostream>
#include <set>
#include <algorithm>

using namespace std;

/*
    LC 850 - Rectangle Area II
    https://leetcode.com/problems/rectangle-area-ii/

    You are given a 2D array of axis-aligned rectangles. Each rectangle[i] = [x_i1, y_i1, x_i2, y_i2] denotes the i^th rectangle where (x_i1, y_i1) are the coordinates of the bottom-left corner, and (x_i2, y_i2) are the coordinates of the top-right corner.

    Calculate the total area covered by all rectangles in the plane. Any area covered by two or more rectangles should only be counted once.

    Return the total area. Since the answer may be too large, return it modulo 10^9 + 7.

    Example 1:

    Input: rectangles = [[0,0,2,2],[1,0,2,3],[1,0,3,1]]
    Output: 6
    Explanation: A total area of 6 is covered by all three rectangles, as illustrated in the picture.
    From (1,1) to (2,2), the green and red rectangles overlap.
    From (1,0) to (2,3), all three rectangles overlap.

    Example 2:

    Input: rectangles = [[0,0,1000000000,1000000000]]
    Output: 49
    Explanation: The answer is 10^18 modulo (10^9 + 7), which is 49.

    Constraints:

    - 1 <= rectangles.length <= 200
    - rectanges[i].length == 4
    - 0 <= x_i1, y_i1, x_i2, y_i2 <= 10^9
    - x_i1 <= x_i2
    - y_i1 <= y_i2
    - All rectangles have non zero area.
*/
/*
    Best solution: sweep line over x + segment tree over compressed y (cnt / len, no lazy)

    Complexity:
        Time:  O(n log n) — sort 2n events, then O(log n) segment-tree update per event.
        Space: O(n) — events, compressed ys and a 4n-node tree.

    Approach (detail):
        1. Each rectangle gives two events: (x1, [y1, y2), +1) and (x2, [y1, y2), -1). Sort by x.
        2. Between two consecutive event x's the set of rectangles spanning the strip is fixed, so
           the strip contributes (x - prev_x) * |union of their y-intervals|. Add it BEFORE
           applying the event at x (the event only changes what lies to the right of x).
        3. Compress all y1/y2 into ys; leaf j is the cell [ys[j], ys[j+1]). Every y-interval is a
           contiguous run of cells, and cells never overlap.
        4. Segment-tree node stores cnt = number of intervals stamped on exactly this node, and
           len = covered length inside the node: cnt > 0 ? full length : (leaf ? 0 : sum of
           children). Insert/erase = range update cnt += ±1 on the O(log n) canonical nodes, then
           recompute len on the way up. The union is len at the root, O(1).
        5. No lazy propagation: every -1 hits exactly the nodes its +1 stamped, only the root is
           queried, and a stamped node ignores its children — children never need the parent's
           stamp. Accumulate the answer mod 1e9+7 (strip area fits in long long).
*/
/*
    Your solution: sweep line + non-lazy (cnt, len) segment tree on compressed y

    Complexity:
        Time:  O(n log n) — sort events, binary search + update_range per event.
        Space: O(n) — events, ys and segtree nodes (4N+1).

    Approach:
        1. Build events {x, y1, y2, d} (d = +1 at x1, -1 at x2), sort by x; collect y's in a set
           and copy to the sorted vector ys (N = ys.size()-1 cells).
        2. segtree.build stores mlen (real length) per node: leaf l covers [ys[l-1], ys[l]).
        3. decompose(y1, y2) maps the interval to cells [L, R] by lower_bound; y_insert / y_erase
           call update_range(L+1, R+1, ±1) (1-based leaves).
        4. update_range adds val to cnt on fully covered nodes and recomputes len with calc_len
           (cnt > 0 -> mlen, leaf -> 0, else sum of children) both there and after recursing.
        5. Per event: curr_area = (x - prev_x) * y_union() mod MOD, total_area = sum_mod(...),
           then apply the insert/erase and move prev_x.

    Verdict: MATCHES BEST — the documented sweep line + non-lazy segment tree.
        Verified against a unit-grid brute force on 3000 random inputs (n <= 5, coords <= 13).
*/
class Solution {
    public:
        int rectangleArea(vector<vector<int>>& rectangles) {
            // với các bài sweepline như này, luôn suy nghĩ theo hướng:
            //  - giả sử ta form tập điểm theo chiều x: (x1, x2, x3, x4..., xn)
            //  - khi đó, với 2 điểm (prev_x: x{i-1} -> curr_x: x{i}), có bao nhiêu rectangles chứa dải này và hợp của dải là bao nhiêu
            //      - simple hoá bài toán Y theo độ dài (điểm y1 = 0)
            //      - khi ta có 1 điểm (x1, x2) thuộc 1 rectangle:
            //          - ta xét d1: +1, thể hiện rằng (x1, T1, +1) đang là điểm bắt đầu (active của 1 hcn-1 độ dài T1)
            //          - khi tới x2 (điểm kết thúc của hcn-1), ta lấy d2:-1, tức ta có điểm: (x2, T1, -1)
            //  - như vậy nếu ta duyệt tại x2:
            //      - area = (x2 - x1) * T1
            //  - tổng quát hoá: giả sử ta duyệt tới điểm xi, ta đang có tập active hcn T
            //      - Tính area của dải active: area = (x - prev_x) * max(active)
            //      - tuỳ vào trạng thái của d, nếu:
            //          d = 1   => điểm khởi đầu của 1 hcn mới => append (h) vào tập active
            //          d = -1  => điểm kết thúc của 1 hcn cũ có độ dài h => erase (h) khỏi tập active
            //  - vậy với bài toán tổng quát hơn: y1 <= y2 bất kì thì sao?
            //      => vẫn tương tự schema trên, nhưng mà: max(active) bây giờ chuyển thành union(active)
            //      => cách chuyển active: dùng segment tree trên chiều Y, mỗi node lá là 1 cell đoạn con: (yi, yi+1) liên tiếp.
            //      => node chứa 2 giá trị: counter và len, với counter: đoạn đó được cover bởi bao nhiêu intervals và len: độ dài valid interval chứa trong đoạn đó
            //      => khi ta insert/erase 1 interval X => ta phân rã ra được [L...R] node cần update
            //      => bài toán:    - update_range(L,R)
            //                      - query(1...N) (query root)
            //      => thông thường, ta sẽ nghĩ tới lazy segtree, nhưng ở bài này không cần thiết vì:
            //          - khi ta insert và erase đều trên cùng 1 tập (L,R), triệt tiêu nhau, do đó không cần phải quan tâm tới node con khi chỉ cần node cha là đã đủ thông tin
            //          - ta chỉ query điểm gốc, là điểm mà luôn được cập nhật thông tin mỗi khi insert/erase, không bao giờ cần mò tới node con là node mà có thể lazy cần tới
            const int MOD = 1e9 + 7;

            struct event {
                int x;
                int y1, y2;
                int d;
            };

            struct segtree {
                struct node {
                    int cnt{0};
                    int len{0};
                    int mlen{0};
                };

                int N;
                vector<node> nodes;

                segtree(int n) : N(n), nodes(4*n+1){}

                void build(int t, int l, int r, vector<int>& val){
                    // 1: val[0]->val[1]
                    // 2: val[1]->val[2]
                    // l: val[l-1]->val[l]
                    if (l == r){
                        nodes[t].mlen = val[l] - val[l-1];
                        return;
                    }
                    int m = (l+r)/2;
                    build(2*t,l,m,val);
                    build(2*t+1,m+1,r,val);
                    nodes[t].mlen = nodes[2*t].mlen + nodes[2*t+1].mlen;
                }

                void init(vector<int>& ys){
                    build(1,1,N,ys);
                }

                int query_root(){
                    return nodes[1].len;
                }

                int calc_len(int t, int l, int r){
                    if (nodes[t].cnt > 0) return nodes[t].mlen; // tồn tại trường hợp: t.cnt > 0, nhưng mà 2*t.cnt = 0 hoặc 2*t+1.cnt = 0
                    if (l == r) return 0;
                    return nodes[2*t].len + nodes[2*t+1].len;
                }

                void update_range(int t, int l, int r, int X, int Y, int val){
                    // case 1: [l...r][X...Y] or [X...Y][l...r]
                    if (r < X || Y < l) return;
                    // case 2: [X...l...r...Y]
                    if (X <= l && r <= Y){
                        nodes[t].cnt += val;
                        nodes[t].len = calc_len(t, l, r);
                        return;
                    }
                    // case 3: overlap
                    int m = (l+r)/2;
                    update_range(2*t,l,m,X,Y,val);
                    update_range(2*t+1,m+1,r,X,Y,val);
                    nodes[t].len = calc_len(t, l, r);
                }

                void update_range(int L, int R, int val){
                    // leaf[L...R] += val
                    update_range(1, 1, N, L, R, val);
                }
            };

            vector<event> events;
            set<int> y_axis;
            for (auto& rect : rectangles){
                int x1 = rect[0], y1 = rect[1], x2 = rect[2], y2 = rect[3];
                events.push_back({x1, y1, y2, 1});  // new active interval
                events.push_back({x2, y1, y2, -1}); // deactivate interval
                y_axis.insert(y1);
                y_axis.insert(y2);
            }
            sort(events.begin(), events.end(), [](const event& lhs, const event& rhs){
                return lhs.x < rhs.x;
            });

            vector<int> ys(y_axis.begin(), y_axis.end());
            int K = ys.size();
            int N = K - 1;

            segtree tree(N);
            tree.init(ys);

            int total_area = 0;
            int prev_x = events[0].x;

            auto decompose = [&](int y1, int y2) -> pair<int, int> {
                int L = lower_bound(ys.begin(), ys.end(), y1) - ys.begin();
                int R = lower_bound(ys.begin(), ys.end(), y2) - ys.begin() - 1;
                return {L, R};
            };

            auto y_union = [&]() -> int {
                return tree.query_root();
            };

            auto y_insert = [&](int L, int R) -> void {
                return tree.update_range(L+1, R+1, 1);
            };

            auto y_erase = [&](int L, int R) -> void {
                return tree.update_range(L+1, R+1, -1);
            };

            auto sum_mod = [&](int a, int b) -> int {
                int c = a + b;
                if (c >= MOD) c -= MOD;
                return c;
            };

            for (auto& event : events){
                int x = event.x, y1 = event.y1, y2 = event.y2, d = event.d;
                auto [L, R] = decompose(y1, y2);
                int curr_area = (1LL * (x - prev_x) * y_union()) % MOD;
                total_area = sum_mod(total_area, curr_area);
                // insert or erase current interval
                (d == 1) ? y_insert(L, R) : y_erase(L, R);
                prev_x = x;
            }
            return total_area;
        }
};

auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
}();

int main(){
    Solution sol;
    return 0;
}