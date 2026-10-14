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
    LC 3727 - Maximum Alternating Sum of Squares
    https://leetcode.com/problems/maximum-alternating-sum-of-squares/

    You are given an integer array nums. You may rearrange the elements in any order.

    The alternating score of an array arr is defined as:

    - score = arr[0]^2 - arr[1]^2 + arr[2]^2 - arr[3]^2 + ...

    Return an integer denoting the maximum possible alternating score of nums after rearranging its elements.

    Example 1:

    Input: nums = [1,2,3]
    Output: 12
    Explanation:
    A possible rearrangement for nums is [2,1,3], which gives the maximum alternating score among all possible rearrangements.
    The alternating score is calculated as:
    score = 2^2 - 1^2 + 3^2 = 4 - 1 + 9 = 12

    Example 2:

    Input: nums = [1,-1,2,-2,3,-3]
    Output: 16
    Explanation:
    A possible rearrangement for nums is [-3,-1,-2,1,3,2], which gives the maximum alternating score among all possible rearrangements.
    The alternating score is calculated as:
    score = (-3)^2 - (-1)^2 + (-2)^2 - (1)^2 + (3)^2 - (2)^2 = 9 - 1 + 4 - 1 + 9 - 4 = 16

    Constraints:

    - 1 <= nums.length <= 10^5
    - -4 * 10^4 <= nums[i] <= 4 * 10^4
*/
/*
    Best solution: greedy on absolute values — the ceil(n/2) largest |x| go to the '+' (even)
    positions, the rest to the '-' (odd) positions; pick that split with a linear-time selection
    (nth_element on |x|) or a counting sort over |x| <= 4 * 10^4.

    Complexity:
        Time:  O(n) expected with nth_element (O(n + 4 * 10^4) with counting) — only the split
               matters, not the full order.
        Space: O(1) extra (in-place selection on the array).

    Approach (detail):
        1. The sign of x does not matter because x^2 = |x|^2; replace every value by its square.
        2. A rearrangement only decides which elements land on the ceil(n/2) even positions (added)
           and which on the floor(n/2) odd positions (subtracted); order within a group is irrelevant.
        3. score = (sum of plus-group) - (sum of minus-group) = total - 2 * (minus-group), so it is
           maximised by putting the floor(n/2) smallest squares in the minus group, i.e. the
           ceil(n/2) largest squares in the plus group (exchange argument: swapping a larger value
           into the minus group never increases the score).
        4. Partition around the ceil(n/2)-th largest |x| and return (sum of top squares) - (sum of the rest),
           accumulating in long long (up to 10^5 * 1.6 * 10^9).

    Your solution:
        SUBOPTIMAL — same greedy (abs, top ceil(n/2) squared added, rest subtracted), but it fully
        sorts: O(n log n) time vs O(n) expected with nth_element, O(1) extra space. The log factor is
        irrelevant at n <= 10^5 and it passes comfortably; swapping sort for nth_element would make
        it optimal. Verified against brute force over all permutations for n <= 7.
*/
class Solution {
    public:
        long long maxAlternatingSum(vector<int>& nums) {
            for (auto& num : nums) num = abs(num);
            sort(nums.begin(), nums.end(), std::greater<int>());
            long long res = 0;
            int n = nums.size(), mid = (n+1)/2;
            for (int i = 0; i < mid; ++i) res += 1LL * nums[i] * nums[i];
            for (int i = mid; i < n; ++i) res -= 1LL * nums[i] * nums[i];
            return res;
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