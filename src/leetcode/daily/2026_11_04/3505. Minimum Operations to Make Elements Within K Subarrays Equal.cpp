#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <queue>
#include <algorithm>
#include <climits>

using namespace std;

/*
    LC 3505 - Minimum Operations to Make Elements Within K Subarrays Equal
    https://leetcode.com/problems/minimum-operations-to-make-elements-within-k-subarrays-equal/

    You are given an integer array nums and two integers, x and k. You can perform the following operation any number of times (including zero):

    - Increase or decrease any element of nums by 1.

    Return the minimum number of operations needed to have at least k non-overlapping subarrays of size exactly x in nums, where all elements within each subarray are equal.

    Example 1:

    Input: nums = [5,-2,1,3,7,3,6,4,-1], x = 3, k = 2
    Output: 8
    Explanation:
    - Use 3 operations to add 3 to nums[1] and use 2 operations to subtract 2 from nums[3]. The resulting array is [5, 1, 1, 1, 7, 3, 6, 4, -1].
    - Use 1 operation to add 1 to nums[5] and use 2 operations to subtract 2 from nums[6]. The resulting array is [5, 1, 1, 1, 7, 4, 4, 4, -1].
    - Now, all elements within each subarray [1, 1, 1] (from indices 1 to 3) and [4, 4, 4] (from indices 5 to 7) are equal. Since 8 total operations were used, 8 is the output.

    Example 2:

    Input: nums = [9,-2,-2,-2,1,5], x = 2, k = 2
    Output: 3
    Explanation:
    - Use 3 operations to subtract 3 from nums[4]. The resulting array is [9, -2, -2, -2, -2, 5].
    - Now, all elements within each subarray [-2, -2] (from indices 1 to 2) and [-2, -2] (from indices 3 to 4) are equal. Since 3 operations were used, 3 is the output.

    Constraints:

    - 2 <= nums.length <= 10^5
    - -10^6 <= nums[i] <= 10^6
    - 2 <= x <= nums.length
    - 1 <= k <= 15
    - 2 <= k * x <= nums.length
*/
/*
    Best solution: sliding-window median (two heaps, lazy deletion) + DP over k segments

    Complexity:
        Time:  O(n log x + n*k) — each element enters / leaves the heaps once; DP has n*k states, O(1) each.
        Space: O(n*k) — the dp table (plus O(n) for cost[] and the heaps).

    Approach (detail):
        1. Making a window of x elements equal costs sum |a - m|, minimised when m is the window median.
        2. Slide a window of size x and keep its median with two heaps: lower half (max-heap) and upper
           half (min-heap) whose sizes differ by at most 1. Removing the outgoing element is lazy: record
           it in a counter map and pop it only when it reaches a heap top; keep the logical sizes and
           sums of each half separately so lazily-deleted elements are not counted.
        3. cost[i] for the window ending at i = (sum_upper - med*|upper|) + (med*|lower| - sum_lower).
        4. dp[i][j] = min cost of choosing j non-overlapping windows inside nums[0..i]:
           dp[i][j] = min(dp[i-1][j], dp[i-x][j-1] + cost[i]); dp[*][0] = 0, and the first window
           [0, x-1] starts from "nothing before" (cost 0 when j == 1). Answer dp[n-1][k].
*/
/*
    Your solution: two heaps with lazy deletion for the window cost + DP over (end index, segments)

    Complexity:
        Time:  O(n log x + n*k) — one insertion and one lazy deletion per index, then the n*k DP.
        Space: O(n*k) — dp; cost[], the heaps and the deletion map are O(n).

    Approach:
        1. min_heap = upper half (holds the median), max_heap = lower half; sz1/sz2 and sum1/sum2 are the
           logical sizes and sums. Invariant: sz1 == sz2 or sz1 == sz2 + 1, so the median is
           min_heap.top() (get_median checks sz1 == SZ1 && sz2 == SZ2, i.e. the window is full).
        2. do_insertion pushes val into the upper half if val >= min_heap.top(), else the lower half,
           then rebalance() moves one top across if the invariant broke and calls do_deletion() to pop
           lazily-deleted tops.
        3. lazy_deletion(val) increments deletion[val], charges it to the upper half if
           val >= min_heap.top() else to the lower half, then rebalances.
        4. For every full window ending at i: cost[i] = (sum1 - med*sz1) + (med*sz2 - sum2).
        5. dp[i][j] over nums[0..i]: skip i (dp[i-1][j]) or end a window at i
           (prev = dp[i-x][j-1], or 0 / INF when the window starts at index 0) + cost[i].
           Return dp[n-1][k].

    Verdict: MATCHES BEST — sliding median with two heaps + O(n*k) DP.
        Verified: window medians/costs against sorting each window (20000 random arrays), full answer
        against an independent brute-force cost + DP (20000 random cases) and Example 1.
*/
class Solution {
    public:
        long long minOperations(vector<int>& nums, int x, int k) {
            int n = nums.size();

            int SZ1 = (x + 1) / 2;      // min_heap: nửa trên (chứa trung vị)
            int SZ2 = x / 2;            // max_heap: nửa dưới

            vector<long long> cost(n, -1);  // cost[i]: số thao tác để đưa cửa sổ [i-x+1->i] về trung vị
            priority_queue<int, vector<int>, greater<int>> min_heap;
            priority_queue<int, vector<int>> max_heap;
            unordered_map<int, int> deletion;   // deletion[v]: số bản của v đã bị xoá nhưng còn nằm trong heap
            int sz1 = 0, sz2 = 0;
            long long sum1 = 0, sum2 = 0;     // tổng logic của min_heap / max_heap (không tính phần tử chờ xoá)

            // bất biến: sz1 == sz2 hoặc sz1 == sz2 + 1, đỉnh của 2 heap luôn là phần tử chưa bị xoá

            auto do_deletion = [&]() -> void {
                while (!min_heap.empty() && deletion[min_heap.top()] > 0){
                    --deletion[min_heap.top()];
                    min_heap.pop();
                }
                while (!max_heap.empty() && deletion[max_heap.top()] > 0){
                    --deletion[max_heap.top()];
                    max_heap.pop();
                }
            };

            auto rebalance = [&]() -> void {
                if (sz1 > sz2 + 1){
                    int v = min_heap.top();
                    max_heap.push(v);
                    min_heap.pop();
                    sum1 -= v; sum2 += v;
                    --sz1; ++sz2;
                }
                else if (sz1 < sz2){
                    int v = max_heap.top();
                    min_heap.push(v);
                    max_heap.pop();
                    sum2 -= v; sum1 += v;
                    ++sz1; --sz2;
                }
                do_deletion();
            };

            auto lazy_deletion = [&](int val) -> void {
                ++deletion[val];
                if (val >= min_heap.top()) { --sz1; sum1 -= val; }
                else { --sz2; sum2 -= val; }
                rebalance();
            };

            const long long INF = LLONG_MAX / 4;
            auto get_median = [&]() -> long long {
                if (sz1 == SZ1 && sz2 == SZ2) return min_heap.top();
                return INF;
            };

            auto do_insertion = [&](int val) -> void {
                if (sz1 == 0 || val >= min_heap.top()){
                    min_heap.push(val);
                    ++sz1; sum1 += val;
                }
                else {
                    max_heap.push(val);
                    ++sz2; sum2 += val;
                }
                rebalance();
            };

            for (int i = 0; i < n; ++i){
                do_insertion(nums[i]);
                if (i >= x - 1){
                    long long med = get_median();
                    cost[i] = (sum1 - med*sz1) + (med*sz2 - sum2);  // nửa trên >= med, nửa dưới <= med
                }
                if (i-x+1 >= 0) lazy_deletion(nums[i-x+1]);
            }

            vector<vector<long long>> dp(n, vector<long long>(k + 1, INF));   // j chạy từ 0 đến k nên cần k+1 cột
            for (int i = 0; i < n; ++i) dp[i][0] = 0;                          // chọn 0 đoạn thì không tốn gì

            for (int i = 0; i < n; ++i){
                for (int j = 1; j <= k; ++j){
                    // không có đoạn nào kết thúc tại nums[i]
                    if (i > 0) dp[i][j] = dp[i-1][j];
                    // có một đoạn [i-x+1, i] kết thúc tại nums[i]
                    if (i >= x - 1){
                        long long prev = (i - x >= 0) ? dp[i-x][j-1] : (j == 1 ? 0 : INF);   // trước đoạn này không còn phần tử nào
                        if (prev < INF) dp[i][j] = min(dp[i][j], prev + cost[i]);
                    }
                }
            }
            return dp[n-1][k];

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