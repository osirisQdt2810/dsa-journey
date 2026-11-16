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
#include <deque>
#include <tuple>
#include <functional>

using namespace std;

/*
    LC 3420 - Count Non-Decreasing Subarrays After K Operations
    https://leetcode.com/problems/count-non-decreasing-subarrays-after-k-operations/
    @status: editorial
    @time: 351
    @wa: 1
    @created: 2026-11-16 10:19

    You are given an array nums of n integers and an integer k.

    For each subarray of nums, you can apply up to k operations on it. In each operation, you increment any element of the subarray by 1.

    Note that each subarray is considered independently, meaning changes made to one subarray do not persist to another.

    Return the number of subarrays that you can make non-decreasing ​​​​​after performing at most k operations.

    An array is said to be non-decreasing if each element is greater than or equal to its previous element, if it exists.

    Example 1:

    Input: nums = [6,3,1,2,4,4], k = 7
    Output: 17
    Explanation:
    Out of all 21 possible subarrays of nums, only the subarrays [6, 3, 1], [6, 3, 1, 2], [6, 3, 1, 2, 4] and [6, 3, 1, 2, 4, 4] cannot be made non-decreasing after applying up to k = 7 operations. Thus, the number of non-decreasing subarrays is 21 - 4 = 17.

    Example 2:

    Input: nums = [6,3,1,3,6], k = 4
    Output: 12
    Explanation:
    The subarray [3, 1, 3, 6] along with all subarrays of nums with three or fewer elements, except [6, 3, 1], can be made non-decreasing after k operations. There are 5 subarrays of a single element, 4 subarrays of two elements, and 2 subarrays of three elements except [6, 3, 1], so there are 1 + 5 + 4 + 2 = 12 subarrays that can be made non-decreasing.

    Constraints:

    - 1 <= nums.length <= 10^5
    - 1 <= nums[i] <= 10^9
    - 1 <= k <= 10^9
*/
/*
    Best solution: two pointers (l from right to left) + monotonic deque of (prefix max, count) blocks

    Complexity:
        Time:  O(n) — every block is pushed and popped at most once, and r only moves left.
        Space: O(n) — the deque of blocks (O(1) extra otherwise).

    Approach (detail):
        1. Raising each element up to the largest element before it inside the subarray is the cheapest
           way to make it non-decreasing, so need(l, r) = sum over i in [l, r] of (max(a[l..i]) - a[i]).
           A subarray is good iff need(l, r) <= k.
        2. need only grows when the subarray grows and shrinks when it shrinks, so for each l the good r
           form a prefix [l, r_max], and r_max(l) <= r_max(l + 1): two pointers.
        3. Dropping a[l] on the left changes every later prefix max, but dropping a[r] on the right
           removes just one term. So iterate l from n - 1 down to 0: only ever ADD on the left and
           REMOVE on the right.
        4. Keep the window as consecutive blocks (v, cnt) of elements sharing the same prefix max v;
           v increases from front to back, so back.v = max(a[l..r]).
        5. Adding a[l]: pop front blocks with v <= a[l]; each raises its cnt elements from v to a[l], so
           need += (a[l] - v) * cnt; merge them with a[l] into one block (a[l], total cnt). While
           need > k: remove a[r] (need -= back.v - a[r]), decrement back.cnt (pop at 0), --r.
        6. Every l contributes r - l + 1 good subarrays. Use 64-bit for need and the answer.
*/
/*
    Your solution: reversed two pointers with a monotonic deque of (prefix max, count) blocks

    Complexity:
        Time:  O(n) — amortized: each block is pushed once and popped at most once; r only decreases.
        Space: O(n) — the deque.

    Approach:
        1. dq holds pairs (max_v, cnt) ordered left to right; need is the cost of the window (l..r),
           r starts at n - 1.
        2. For l = n - 1 .. 0: cnt = 1; while nums[l] > dq.front().first, add
           (nums[l] - front.first) * front.second to need, add front.second to cnt, pop_front.
        3. push_front({nums[l], cnt}) — the popped blocks are now represented by this one block.
        4. While need > k: need -= dq.back().first - nums[r]; if --dq.back().second == 0 pop_back;
           --r.
        5. ans += r - l + 1.

    Verdict: MATCHES BEST — the O(n) reversed two-pointer monotonic-deque solution (blocks with equal
        value are not merged on a tie, which does not affect correctness). Verified: 2/2 official
        examples and a brute force on 300000 random arrays (n <= 10), plus n = 10^5 with values up to
        10^9 (need and ans in 64-bit, sanitizers clean).
*/
class Solution {
    public:
        long long countNonDecreasingSubarrays(vector<int>& nums, int k) {
            using max_v = int;
            using cnt_v = int;
            using ii = pair<max_v, cnt_v>;
            deque<ii> dq;

            int n = nums.size();
            long long need = 0;
            int r = n-1;
            long long ans = 0;
            for (int l = n-1; l >= 0; --l){
                // update queue hiện tại từ (l+1->r) về (l->r)
                // needs(l,i) = needs(l+1,i) + max(a[l] - max_v(l+1...i), 0)
                // cho nên chỉ cần update các điểm có a[l] > max_v(l+1...i):
                //      ví dụ:       [3, 3, 4, 4, 6]
                //      -> a[l] = 5: [5, 5, 5, 5, 6] -> tính cnt để bớt việc update
                int cnt = 1;
                while (!dq.empty() && nums[l] > dq.front().first){
                    // // need[i] -> need[i] + a[l] - v[i]
                    // need += nums[l] - dq.front().first;
                    // ++cnt;

                    // bởi vì bây giờ đã lưu dạng cnt => phải dùng tích
                    need += 1LL * (nums[l] - dq.front().first) * dq.front().second;
                    cnt += dq.front().second;
                    dq.pop_front();
                }
                dq.push_front({nums[l], cnt});

                // shrink r => tự ngầm hiểu r và cnt
                while (need > k){
                    need -= dq.back().first - nums[r];
                    if (--dq.back().second == 0) dq.pop_back();
                    --r;
                }

                ans += 1LL * (r - l + 1);
            }
            return ans;
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
    auto check = [&](int idx, long long got, long long expected) {
        ++total;
        if (got == expected) {
            ++passed;
            cout << "Example " << idx << ": PASS\n";
        } else {
            cout << "Example " << idx << ": FAIL  got=" << got << "  expected=" << expected << "\n";
        }
    };
    {
        Solution sol;
        vector<int> nums = {6, 3, 1, 2, 4, 4};
        int k = 7;
        check(1, sol.countNonDecreasingSubarrays(nums, k), 17LL);
    }
    {
        Solution sol;
        vector<int> nums = {6, 3, 1, 3, 6};
        int k = 4;
        check(2, sol.countNonDecreasingSubarrays(nums, k), 12LL);
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}
