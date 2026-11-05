#include <vector>
#include <iostream>
#include <set>
#include <climits>
#include <cstdlib>

using namespace std;

/*
    LC 1521 - Find a Value of a Mysterious Function Closest to Target
    https://leetcode.com/problems/find-a-value-of-a-mysterious-function-closest-to-target/

    Winston was given the above mysterious function func. He has an integer array arr and an integer target and he wants to find the values l and r that make the value |func(arr, l, r) - target| minimum possible.

    Return the minimum possible value of |func(arr, l, r) - target|.

    Notice that func should be called with the values l and r where 0 <= l, r < arr.length.

    Example 1:

    Input: arr = [9,12,3,7,15], target = 5
    Output: 2
    Explanation: Calling func with all the pairs of [l,r] = [[0,0],[1,1],[2,2],[3,3],[4,4],[0,1],[1,2],[2,3],[3,4],[0,2],[1,3],[2,4],[0,3],[1,4],[0,4]], Winston got the following results [9,12,3,7,15,8,0,3,7,0,0,3,0,0,0]. The value closest to 5 is 7 and 3, thus the minimum difference is 2.

    Example 2:

    Input: arr = [1000000,1000000,1000000], target = 1
    Output: 999999
    Explanation: Winston called the func with all possible values of [l,r] and he always got 1000000, thus the min difference is 999999.

    Example 3:

    Input: arr = [1,2,4,8,16], target = 0
    Output: 0

    Constraints:

    - 1 <= arr.length <= 10^5
    - 1 <= arr[i] <= 10^6
    - 0 <= target <= 10^7
*/
/*
    Best solution: rolling set of distinct AND values of subarrays ending at r

    Complexity:
        Time:  O(n log V) — at most ~log2(V) + 1 (about 21 for V = 10^6) distinct values per right end.
        Space: O(log V) — only the current set of AND values.

    Approach (detail):
        1. func(arr, l, r) is the bitwise AND of arr[l..r]. Fix r and move l to the left: the AND can only
           lose bits, so it is non-increasing and takes at most log2(V) + 1 distinct values.
        2. Keep cur = the distinct ANDs of all subarrays ending at r - 1. For r:
           cur = { x & arr[r] : x in cur } ∪ { arr[r] } (duplicates merge, so |cur| stays <= ~21).
        3. Every subarray's AND appears in some cur, so ans = min over all r and all v in cur of
           |v - target|.
        4. Alternative with the same monotonicity: for fixed r, AND(l..r) is non-decreasing in l, so binary
           search l with a range-AND structure (sparse table O(1) query -> O(n log n)).
*/
/*
    Your solution: binary_search_and_segtree — range-AND segment tree + binary search on l for each r

    Complexity:
        Time:  O(n log^2 n) — for each right end i, a binary search of O(log n) steps, each an O(log n) query.
        Space: O(n) — the segment tree.

    Approach:
        1. segtree stores AND per node (build from arr, 1-based); query(l, r) returns the AND of the range,
           using nil = -1 for an empty side.
        2. For each right end i: binary search m in [0, i] on val = tree.query(m+1, i+1), which is
           non-decreasing in m. val < target -> record target - val and move right (l = m+1);
           otherwise record val - target and move left (r = m-1). The two values around target are both
           visited, so ans holds the best for this i.
        3. res = min over i of ans.

        Also in the file (not called): bruteforce_value_approach — the rolling set of distinct ANDs ending at
        r (set<int> curr / next), O(n log V log log V); this is the best solution's idea.

    Verdict: SUBOPTIMAL — O(n log^2 n) vs O(n log V) for the rolling AND set; passes easily at n <= 10^5.
        Verified against an O(n^2) brute force on 20000 random cases.
*/
class Solution {
    public:
        int closestToTarget(vector<int>& arr, int target) {
            // very easy if we have idea:
            //  - tại 1 thời điểm r, số giá trị tối đa có thể có trong [1...r] chỉ luôn là <= 20, vì: mỗi khi ta and arr[r] với arr[l] giảm dần, tối đa 1 bit bị giảm đi
            //  - giá trị của số nhỏ hơn 20 bit => có tối đa 20 giá trị có thể có
            int n = arr.size();
            auto bruteforce_value_approach = [&]() -> int {
                set<int> curr;
                int res = INT_MAX;
                for (int r = 0; r < n; ++r){
                    // update cur ended by arr[r]
                    set<int> next;
                    for (int num : curr){
                        next.insert(num & arr[r]);
                    }
                    next.insert(arr[r]);

                    for (int num : next){
                        res = min(res, abs(num - target));
                    }
                    swap(next, curr);
                }
                return res;
            };

            // 1 cách khác cũng dễ tưởng tượng hơn đó là: cố định r, ta cần tìm l sao cho abs[value(l,r)-target] min
            // thông thường, nếu value(l,r) không có tính chất gì, bắt buộc phải ít nhất r * TC(calculate_value(l,r))
            // nhưng mà ở đây có chút đặc biệt: value(l,r) đơn điệu tăng dần (vì tính chất của and), do đó ở đây ta có thể dùng BS
            auto binary_search_and_segtree = [&]() -> int {
                struct segtree {
                    int N;
                    vector<int> it;

                    const int nil = -1;

                    segtree(int n) : N(n), it(4*N+1, 0){}

                    void build(int t, int l, int r, vector<int>& arr){
                        if (l==r){
                            it[t] = arr[l-1];
                            return;
                        }
                        int m = (l+r)/2;
                        build(2*t,l,m,arr);
                        build(2*t+1,m+1,r,arr);
                        it[t] = it[2*t] & it[2*t+1];
                    }

                    void build(vector<int>& arr){
                        build(1,1,N,arr);
                    }

                    int query(int t, int l, int r, int X, int Y){
                        // case 1: [l...r][X...Y] or [X...Y][l...r]
                        if (r < X || Y < l) return nil;
                        // case 2: [X...l...r...Y]
                        if (X <= l && r <= Y) return it[t];
                        // case 3: overlap
                        int m = (l+r)/2;
                        int left = query(2*t,l,m,X,Y);
                        int right = query(2*t+1,m+1,r,X,Y);
                        if (left == nil) return right;
                        if (right == nil) return left;
                        return left & right;
                    }
                    int query(int l, int r){
                        return query(1,1,N,l,r);
                    }
                };

                segtree tree(n);
                tree.build(arr);

                int res = INT_MAX;
                for (int i = 0; i < n; ++i){
                    int l = 0, r = i, ans = INT_MAX;
                    while (l <= r){
                        int m = (l+r)/2;
                        int val = tree.query(m+1, i+1);
                        if (val < target){
                            ans = min(ans, target - val);
                            l = m+1;
                        }
                        else {
                            ans = min(ans, val - target);
                            r = m-1;
                        }
                    }
                    res = min(res, ans);
                }
                return res;
            };

            return binary_search_and_segtree();
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
    int passed = 0, total = 0;
    auto check = [&](int id, int got, int expected) -> void {
        ++total;
        if (got == expected){
            ++passed;
            cout << "Example " << id << ": PASS\n";
        }
        else cout << "Example " << id << ": FAIL  got=" << got << "  expected=" << expected << "\n";
    };

    {
        vector<int> arr = {9,12,3,7,15};
        check(1, Solution().closestToTarget(arr, 5), 2);
    }
    {
        vector<int> arr = {1000000,1000000,1000000};
        check(2, Solution().closestToTarget(arr, 1), 999999);
    }
    {
        vector<int> arr = {1,2,4,8,16};
        check(3, Solution().closestToTarget(arr, 0), 0);
    }

    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}