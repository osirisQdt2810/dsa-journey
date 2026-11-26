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
#include <cmath>
#include <tuple>
#include <functional>

using namespace std;

/*
    LC 3934 - Smallest Unique Subarray
    https://leetcode.com/problems/smallest-unique-subarray/
    @status: self
    @created: 2026-11-20 09:31

    You are given an integer array nums.

    Find the minimum length of a subarray that is not identical to any other subarray in nums.

    Return an integer denoting the minimum possible length of such a subarray.

    Two subarrays are considered identical if they have the same length and the same elements in corresponding positions.

    Example 1:

    Input: nums = [3,3,3]
    Output: 3
    Explanation:
    - Subarrays of length 1: [3] → appears 3 times
    - Subarrays of length 2: [3, 3] → appears 2 times
    - Subarrays of length 3: [3, 3, 3] → appears once
    The subarray [3, 3, 3] is unique, so the smallest unique subarray length is 3.

    Example 2:

    Input: nums = [2,1,2,3,3]
    Output: 1
    Explanation:
    Subarrays of length 1:
    - [2] → appears 2 times
    - [1] → appears once
    - [3] → appears 2 times
    The subarray [1] is unique, so the smallest unique subarray length is 1.

    Example 3:

    Input: nums = [1,1,2,2,1]
    Output: 2
    Explanation:
    Subarrays of length 1:
    - [1] → appears 3 times
    - [2] → appears 2 times
    Subarrays of length 2:
    - [1, 1] → appears once
    - [1, 2] → appears once
    - [2, 2] → appears once
    - [2, 1] → appears once
    There is at least one subarray of length 2 that is unique, so the smallest unique subarray length is 2.

    Constraints:

    - 1 <= nums.length <= 10^5
    - 1 <= nums[i] <= 10^5
*/
/*
    Best solution: suffix automaton (or suffix array + LCP) — shortest substring occurring once

    Complexity:
        Time:  O(n) expected with a hash-map suffix automaton (O(n log n) with a suffix array + LCP).
        Space: O(n) — automaton states / suffix array.

    Approach (detail):
        1. A substring is unique iff it occurs exactly once.
        2. Suffix automaton: a state with exactly one end position (endpos size 1) represents substrings that occur once.
        3. The shortest such substring of that state has length len(link(state)) + 1.
        4. The answer is the minimum of len(link) + 1 over all states with endpos size 1 (the whole array is always unique).
        5. Equivalent suffix-array view: for suffix i the shortest unique prefix has length max(lcp with both neighbours) + 1,
           valid if it fits in n - i; take the minimum over all i.
*/
/*
    Your solution: binary search on length m, rolling polynomial hash with counts, check for a count-1 window

    Complexity:
        Time:  O(n log n) expected — log n binary-search steps, each one rolling pass plus an unordered_map.
        Space: O(n) — hash counts per length.

    Approach:
        1. If a length-m subarray is unique, any longer one containing it is unique, so unique-existence is monotone in m.
        2. Binary search m in [1, n]; has_unique_subarray(m) hashes every window with one polynomial hash
           (base 103, mod 1e9+7) using H and P1m = base^(m-1).
        3. Count each hash in hmap; the check is true iff some hash has count 1.
        4. On success ans = m and r = m - 1, else l = m + 1; return ans.

    Verdict: SUBOPTIMAL — O(n log n) probabilistic vs O(n) deterministic; a single 1e9+7 hash can collide, so two
        different windows may merge and hide a unique one. Verified against brute force (all lengths, vector keys)
        on 30000 random small arrays, no mismatch.
*/
class Solution {
    public:
        int smallestUniqueSubarray(vector<int>& nums) {
            int n = nums.size(), l = 1, r = n, ans = n;
            auto has_unique_subarray = [&](int m){
                // a0 * p^m-1 + a1 * p^m-2 + ... + a(m-1) * p^0
                const int P1 = 103, M1 = 1e9 + 7;
                using hash = long long;
                using counter = int;
                unordered_map<hash, counter> hmap;
                hash H1 = 0;
                hash P1m = 1;
                for (int i = 0; i < m; ++i){
                    H1 = (1LL * P1 * H1 % M1 + nums[i]) % M1;
                    if (i > 0) P1m = (P1m * P1) % M1;
                }
                hmap[H1] = 1;
                for (int i = m; i < n; ++i){
                    H1 = (H1 - P1m * nums[i-m] % M1 + M1) % M1;
                    H1 = (H1 * P1 % M1 + nums[i]) % M1;
                    ++hmap[H1];
                }

                for (auto& [H, C] : hmap){
                    if (C == 1) return true;
                }
                return false;
            };

            while (l <= r){
                int m = (l+r)/2;
                if (has_unique_subarray(m)){
                    ans = m;
                    r = m-1;
                }
                else {
                    l = m+1;
                }
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
    // test-lc:end
    return 0;
}
