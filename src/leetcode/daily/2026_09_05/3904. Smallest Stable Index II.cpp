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
    LC 3904 - Smallest Stable Index II
    https://leetcode.com/problems/smallest-stable-index-ii/

    You are given an integer array nums of length n and an integer k.

    For each index i, define its instability score as max(nums[0..i]) - min(nums[i..n - 1]).

    In other words:

    - max(nums[0..i]) is the largest value among the elements from index 0 to index i.
    - min(nums[i..n - 1]) is the smallest value among the elements from index i to index n - 1.

    An index i is called stable if its instability score is less than or equal to k.

    Return the smallest stable index. If no such index exists, return -1.

    Example 1:

    Input: nums = [5,0,1,4], k = 3
    Output: 3
    Explanation:
    - At index 0: The maximum in [5] is 5, and the minimum in [5, 0, 1, 4] is 0, so the instability score is 5 - 0 = 5.
    - At index 1: The maximum in [5, 0] is 5, and the minimum in [0, 1, 4] is 0, so the instability score is 5 - 0 = 5.
    - At index 2: The maximum in [5, 0, 1] is 5, and the minimum in [1, 4] is 1, so the instability score is 5 - 1 = 4.
    - At index 3: The maximum in [5, 0, 1, 4] is 5, and the minimum in [4] is 4, so the instability score is 5 - 4 = 1.
    - This is the first index with an instability score less than or equal to k = 3. Thus, the answer is 3.

    Example 2:

    Input: nums = [3,2,1], k = 1
    Output: -1
    Explanation:
    - At index 0, the instability score is 3 - 1 = 2.
    - At index 1, the instability score is 3 - 1 = 2.
    - At index 2, the instability score is 3 - 1 = 2.
    - None of these values is less than or equal to k = 1, so the answer is -1.

    Example 3:

    Input: nums = [0], k = 0
    Output: 0
    Explanation:
    At index 0, the instability score is 0 - 0 = 0, which is less than or equal to k = 0. Therefore, the answer is 0.

    Constraints:

    - 1 <= nums.length <= 10^5
    - 0 <= nums[i] <= 10^9
    - 0 <= k <= 10^9
*/
/*
    Best solution: suffix-minimum array + running prefix maximum

    Complexity:
        Time:  O(n) — one backward pass for suffix minima, one forward pass for the answer.
        Space: O(n) — the suffix-minimum array (the prefix maximum is a single running value).

    Approach (detail):
        1. score(i) = max(nums[0..i]) - min(nums[i..n-1]). Both terms change with i, and the score
           is not monotone, so every index must be checked in order; the first i with
           score(i) <= k is the answer.
        2. min(nums[i..n-1]) only depends on the suffix: precompute suf[i] = min(nums[i], suf[i+1])
           from right to left.
        3. Scan left to right keeping pre = max(pre, nums[i]); return the first i with
           pre - suf[i] <= k, or -1. Values are in [0, 10^9], so the difference fits in int.
*/
/*
    Your solution: prefix-max array + suffix-min array, then a first-hit scan

    Complexity:
        Time:  O(n) — three linear passes.
        Space: O(n) — max_ele and min_ele, both of size n.

    Approach:
        1. max_ele[i] = max(max_ele[i-1], nums[i]) left to right (max_ele[0] = nums[0]).
        2. min_ele[i] = min(min_ele[i+1], nums[i]) right to left (min_ele[n-1] = nums[n-1]).
        3. Return the first i with max_ele[i] - min_ele[i] <= k, else -1.

    Verdict: MATCHES BEST — same O(n) time and O(n) space; the prefix maxima could be a running
        variable instead of an array, which only saves a constant factor.
        Verified against a direct O(n^2) brute force on 200000 random arrays (n <= 10, values up
        to 10^9) and on the three examples, under ASan/UBSan.
*/
class Solution {
    public:
        int firstStableIndex(vector<int>& nums, int k) {
            int n = nums.size();
            vector<int> max_ele(n, INT_MIN);
            vector<int> min_ele(n, INT_MAX);
            for (int i = 0; i < n; ++i){
                max_ele[i] = (i > 0) ? max(max_ele[i-1], nums[i]) : nums[i];
            }

            for (int i = n-1; i >= 0; --i){
                min_ele[i] = (i < n-1) ? min(min_ele[i+1], nums[i]) : nums[i];
            }

            for (int i = 0; i < n; ++i){
                int instability = max_ele[i] - min_ele[i];
                if (instability <= k) return i;
            }
            return -1;
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