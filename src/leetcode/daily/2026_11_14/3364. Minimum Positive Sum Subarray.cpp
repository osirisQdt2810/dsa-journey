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
#include <tuple>
#include <functional>

using namespace std;

/*
    LC 3364 - Minimum Positive Sum Subarray
    https://leetcode.com/problems/minimum-positive-sum-subarray/
    @status: self
    @time: 10
    @wa: 0

    You are given an integer array nums and two integers l and r. Your task is to find the minimum sum of a subarray whose size is between l and r (inclusive) and whose sum is greater than 0.

    Return the minimum sum of such a subarray. If no such subarray exists, return -1.

    A subarray is a contiguous non-empty sequence of elements within an array.

    Example 1:

    Input: nums = [3, -2, 1, 4], l = 2, r = 3
    Output: 1
    Explanation:
    The subarrays of length between l = 2 and r = 3 where the sum is greater than 0 are:
    - [3, -2] with a sum of 1
    - [1, 4] with a sum of 5
    - [3, -2, 1] with a sum of 2
    - [-2, 1, 4] with a sum of 3
    Out of these, the subarray [3, -2] has a sum of 1, which is the smallest positive sum. Hence, the answer is 1.

    Example 2:

    Input: nums = [-2, 2, -3, 1], l = 2, r = 3
    Output: -1
    Explanation:
    There is no subarray of length between l and r that has a sum greater than 0. So, the answer is -1.

    Example 3:

    Input: nums = [1, 2, 3, 4], l = 2, r = 4
    Output: 3
    Explanation:
    The subarray [1, 2] has a length of 2 and the minimum sum greater than 0. So, the answer is 3.

    Constraints:

    - 1 <= nums.length <= 100
    - 1 <= l <= r <= nums.length
    - -1000 <= nums[i] <= 1000
*/
/*
    Best solution: prefix sums + ordered set over the sliding window of valid start prefixes

    Complexity:
        Time:  O(n log n) — each prefix is inserted into / erased from the set once and queried once.
        Space: O(n) — the prefix array and the set.

    Approach (detail):
        1. Let P[0] = 0, P[j] = nums[0] + ... + nums[j-1]. The sum of nums[i..j-1] is P[j] - P[i], with
           length j - i that must lie in [l, r], i.e. i in [j - r, j - l].
        2. Sweep j = l..n, keeping in an ordered multiset exactly the prefixes P[i] for i in that window
           (insert P[j-l] when j advances, erase P[j-r-1] when it falls out).
        3. The smallest positive P[j] - P[i] comes from the LARGEST P[i] strictly below P[j]:
           it = set.lower_bound(P[j]); if it != begin, candidate = P[j] - *prev(it).
        4. Keep the minimum candidate; return -1 if none was found.
        With n <= 100 an O(n^2) scan is just as practical; the set version is the general-n optimum.
*/
/*
    Your solution: enumerate every start i and extend the length from l to r with a running sum

    Complexity:
        Time:  O(n * r) = O(n^2) — for each start, up to r extensions.
        Space: O(1).

    Approach:
        1. For each start i with at least l elements left, pre-sum the first l-1 elements.
        2. Extend sz from l to r (while in bounds), adding nums[i+sz-1]; whenever sum > 0 and
           sum < minsum, record it.
        3. Return -1 if minsum stayed INT_MAX, else minsum.

    Verdict: SUBOPTIMAL — correct, but O(n^2) vs O(n log n); irrelevant at n <= 100.
        Verified: 3/3 official examples and a brute force on 200000 random arrays (n <= 12,
        values in [-10, 10], random l <= r).
*/
class Solution {
    public:
        int minimumSumSubarray(vector<int>& nums, int l, int r) {
            int n = nums.size();
            int minsum = INT_MAX;
            for (int i = 0; i + (l-1) < n; ++i){
                int sum = 0;
                for (int sz = 1; sz < l && i + (sz-1) < n; ++sz) sum += nums[i + (sz-1)];
                for (int sz = l; sz <= r && i + (sz-1) < n; ++sz){
                    sum += nums[i + (sz-1)];
                    if (sum > 0 && sum < minsum){
                        minsum = sum;
                    }
                }
            }
            return (minsum == INT_MAX) ? -1 : minsum;
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
    auto check = [&](int idx, int got, int expected) {
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
        vector<int> nums = {3, -2, 1, 4};
        int l = 2, r = 3;
        check(1, sol.minimumSumSubarray(nums, l, r), 1);
    }
    {
        Solution sol;
        vector<int> nums = {-2, 2, -3, 1};
        int l = 2, r = 3;
        check(2, sol.minimumSumSubarray(nums, l, r), -1);
    }
    {
        Solution sol;
        vector<int> nums = {1, 2, 3, 4};
        int l = 2, r = 4;
        check(3, sol.minimumSumSubarray(nums, l, r), 3);
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}
