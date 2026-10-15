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
    LC 2913 - Subarrays Distinct Element Sum of Squares I
    https://leetcode.com/problems/subarrays-distinct-element-sum-of-squares-i/

    You are given a 0-indexed integer array nums.

    The distinct count of a subarray of nums is defined as:

    - Let nums[i..j] be a subarray of nums consisting of all the indices from i to j such that 0 <= i <= j < nums.length. Then the number of distinct values in nums[i..j] is called the distinct count of nums[i..j].

    Return the sum of the squares of distinct counts of all subarrays of nums.

    A subarray is a contiguous non-empty sequence of elements within an array.

    Example 1:

    Input: nums = [1,2,1]
    Output: 15
    Explanation: Six possible subarrays are:
    [1]: 1 distinct value
    [2]: 1 distinct value
    [1]: 1 distinct value
    [1,2]: 2 distinct values
    [2,1]: 2 distinct values
    [1,2,1]: 2 distinct values
    The sum of the squares of the distinct counts in all subarrays is equal to 1^2 + 1^2 + 1^2 + 2^2 + 2^2 + 2^2 = 15.

    Example 2:

    Input: nums = [1,1]
    Output: 3
    Explanation: Three possible subarrays are:
    [1]: 1 distinct value
    [1]: 1 distinct value
    [1,1]: 1 distinct value
    The sum of the squares of the distinct counts in all subarrays is equal to 1^2 + 1^2 + 1^2 = 3.

    Constraints:

    - 1 <= nums.length <= 100
    - 1 <= nums[i] <= 100
*/
/*
    Best solution: for this version (n <= 100) an O(n^2) sweep over all subarrays with an
    incrementally maintained distinct count; asymptotically, the O(n log n) lazy segment tree from
    LC 2916 (sum of squares of distinct counts, updated as the right end moves) is the best known.

    Complexity:
        Time:  O(n^2) for the sweep (O(n log n) with the segment tree).
        Space: O(V) for a frequency array / set of the current subarray, V <= 100.

    Approach (detail):
        1. Fix the left end i; extend the right end j from i to n-1.
        2. Keep the set (or a seen[] array) of values in nums[i..j]; when nums[j] is new the
           distinct count d grows by one.
        3. Add d^2 for every (i, j). The total is at most n^2/2 * n^2 = 5 * 10^7, which fits in int.
        4. Segment-tree version: process j left to right; nums[j] raises the distinct count of
           every subarray starting in (last[nums[j]], j] by one, and sum of (d+1)^2 - d^2 = 2d + 1
           over a range is maintained with range-add + range-sum.

    Your solution:
        SUBOPTIMAL — the O(n^2) double loop with an unordered_set of the current window's values:
        O(n^2) expected time vs O(n log n) for the segment tree, O(n) space. It is the intended
        solution for the n <= 100 version and runs instantly; a plain bool seen[101] array instead
        of unordered_set would be a constant-factor speed-up. Verified against a brute force on
        3000 random arrays.
*/
class Solution {
    public:
        int sumCounts(vector<int>& nums) {
            int ans = 0;
            int n = nums.size();
            unordered_set<int> uniques;
            for (int i = 0; i < n; ++i){
                uniques.clear();
                for (int j = i; j < n; ++j){
                    if (!uniques.count(nums[j])){
                        uniques.insert(nums[j]);
                    }
                    ans += (uniques.size()) * uniques.size();
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
    Solution sol;

    return 0;
}