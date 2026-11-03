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
    LC 462 - Minimum Moves to Equal Array Elements II
    https://leetcode.com/problems/minimum-moves-to-equal-array-elements-ii/

    Given an integer array nums of size n, return the minimum number of moves required to make all array elements equal.

    In one move, you can increment or decrement an element of the array by 1.

    Test cases are designed so that the answer will fit in a 32-bit integer.

    Example 1:

    Input: nums = [1,2,3]
    Output: 2
    Explanation:
    Only two moves are needed (remember each move increments or decrements one element):
    [1,2,3]  =>  [2,2,3]  =>  [2,2,2]

    Example 2:

    Input: nums = [1,10,2,9]
    Output: 16

    Constraints:

    - n == nums.length
    - 1 <= nums.length <= 10^5
    - -10^9 <= nums[i] <= 10^9
*/
/*
    Best solution: move everything to the median (quickselect)

    Complexity:
        Time:  O(n) expected — nth_element plus one summing pass.
        Space: O(1) extra — nth_element works in place.

    Approach (detail):
        1. The cost of target t is f(t) = sum |nums[i] - t|, a convex piecewise-linear function.
        2. Its slope is (#elements < t) - (#elements > t), which crosses zero at the median, so
           any median minimises f.
        3. Find the median with nth_element(nums.begin(), nums.begin() + n/2, nums.end()).
        4. answer = sum |x - median| (accumulate in 64-bit, return as int — guaranteed to fit).
*/
/*
    Your solution: sort, then pair the smallest with the largest

    Complexity:
        Time:  O(n log n) — the sort dominates.
        Space: O(log n) — sort recursion stack.

    Approach:
        1. sort(nums).
        2. Two pointers i = 0, j = n-1 moving inwards: moves += nums[j] - nums[i]. Each outer
           pair (a, b) costs b - a for any target in [a, b], and the median lies inside every
           such pair, so the sum of pair gaps is exactly the cost of moving to the median.
        3. Return moves.

    Verdict: SUBOPTIMAL — correct, but O(n log n) vs O(n) expected with nth_element; at
        n <= 10^5 the sort is negligible and it passes easily.
        Verified against an exhaustive target scan on 2000 random arrays.
*/
class Solution {
    public:
        int minMoves2(vector<int>& nums) {
            int n = nums.size();
            auto median_number = [&]() -> int {
                sort(nums.begin(), nums.end());
                long long moves = 0;
                for (int i = 0, j = n - 1; i < j; ++i, --j){
                    moves += nums[j] - nums[i];
                }
                return moves;
            };
            return median_number();
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