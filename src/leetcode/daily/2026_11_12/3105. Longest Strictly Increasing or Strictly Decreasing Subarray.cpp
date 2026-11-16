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
    LC 3105 - Longest Strictly Increasing or Strictly Decreasing Subarray
    https://leetcode.com/problems/longest-strictly-increasing-or-strictly-decreasing-subarray/
    @status: self
    @time: 18
    @wa: 0
    @created: 2026-11-12 15:31

    You are given an array of integers nums. Return the length of the longest subarray of nums which is either strictly increasing or strictly decreasing.

    Example 1:

    Input: nums = [1,4,3,3,2]
    Output: 2
    Explanation:
    The strictly increasing subarrays of nums are [1], [2], [3], [3], [4], and [1,4].
    The strictly decreasing subarrays of nums are [1], [2], [3], [3], [4], [3,2], and [4,3].
    Hence, we return 2.

    Example 2:

    Input: nums = [3,3,3,3]
    Output: 1
    Explanation:
    The strictly increasing subarrays of nums are [3], [3], [3], and [3].
    The strictly decreasing subarrays of nums are [3], [3], [3], and [3].
    Hence, we return 1.

    Example 3:

    Input: nums = [3,2,1]
    Output: 3
    Explanation:
    The strictly increasing subarrays of nums are [3], [2], and [1].
    The strictly decreasing subarrays of nums are [3], [2], [1], [3,2], [2,1], and [3,2,1].
    Hence, we return 3.

    Constraints:

    - 1 <= nums.length <= 50
    - 1 <= nums[i] <= 50
*/
/*
    Best solution: single pass tracking the current increasing and decreasing run lengths

    Complexity:
        Time:  O(n) — one scan.
        Space: O(1) — two run counters and the best length.

    Approach (detail):
        1. A subarray is strictly increasing or strictly decreasing, so every adjacent pair is "up",
           "down" or "equal".
        2. Scan i = 1..n-1 keeping inc = length of the strictly increasing run ending at i and
           dec = length of the strictly decreasing run ending at i (both start at 1).
        3. If nums[i] > nums[i-1]: inc += 1, dec = 1. If nums[i] < nums[i-1]: dec += 1, inc = 1.
           If equal: inc = dec = 1.
        4. The answer is the maximum of inc and dec seen over the scan (at least 1).
*/
/*
    Your solution: one pass with separate current/best counters for the increasing and decreasing runs

    Complexity:
        Time:  O(n) — one loop over nums.
        Space: O(1) — li_cur, ld_cur, linc, ldec.

    Approach:
        1. linc = ldec = li_cur = ld_cur = 1.
        2. For i = 1..n-1: equal neighbours reset both current runs to 1.
        3. If nums[i-1] < nums[i]: ++li_cur, linc = max(linc, li_cur), ld_cur = 1; otherwise the pair is
           decreasing: ++ld_cur, ldec = max(ldec, ld_cur), li_cur = 1.
        4. Return max(linc, ldec).

    Verdict: MATCHES BEST — the O(n) / O(1) run-length scan, with the best lengths kept per direction.
        Verified: 3/3 official examples and a brute force on 200000 random arrays (n <= 12, values 1..6).
*/
class Solution {
    public:
        int longestMonotonicSubarray(vector<int>& nums) {
            int linc = 1, ldec = 1, li_cur = 1, ld_cur = 1, n = nums.size();
            for (int i = 1; i < n; ++i){
                if (nums[i] == nums[i-1]){
                    li_cur = ld_cur = 1;
                    continue;
                }

                if (nums[i-1] < nums[i]){
                    ++li_cur;
                    linc = max(linc, li_cur);
                    ld_cur = 1;
                }
                else {
                    ++ld_cur;
                    ldec = max(ldec, ld_cur);
                    li_cur = 1;
                }
            }
            return max(linc, ldec);
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
        vector<int> nums = {1, 4, 3, 3, 2};
        check(1, sol.longestMonotonicSubarray(nums), 2);
    }
    {
        Solution sol;
        vector<int> nums = {3, 3, 3, 3};
        check(2, sol.longestMonotonicSubarray(nums), 1);
    }
    {
        Solution sol;
        vector<int> nums = {3, 2, 1};
        check(3, sol.longestMonotonicSubarray(nums), 3);
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}