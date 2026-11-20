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
    LC 2567 - Minimum Score by Changing Two Elements
    https://leetcode.com/problems/minimum-score-by-changing-two-elements/
    @status: self
    @time: 25
    @wa: 1
    @created: 2026-11-20 09:02

    You are given an integer array nums.

    - The low score of nums is the minimum absolute difference between any two integers.
    - The high score of nums is the maximum absolute difference between any two integers.
    - The score of nums is the sum of the high and low scores.

    Return the minimum score after changing two elements of nums.

    Example 1:

    Input: nums = [1,4,7,8,5]
    Output: 3
    Explanation:
    - Change nums[0] and nums[1] to be 6 so that nums becomes [6,6,7,8,5].
    - The low score is the minimum absolute difference: |6 - 6| = 0.
    - The high score is the maximum absolute difference: |8 - 5| = 3.
    - The sum of high and low score is 3.

    Example 2:

    Input: nums = [1,4,3]
    Output: 0
    Explanation:
    - Change nums[1] and nums[2] to 1 so that nums becomes [1,1,1].
    - The sum of maximum absolute difference and minimum absolute difference is 0.

    Constraints:

    - 3 <= nums.length <= 10^5
    - 1 <= nums[i] <= 10^9
*/
/*
    Best solution: sort-free selection of the 3 smallest / 3 largest (or sort), then take the best of three trims

    Complexity:
        Time:  O(n) — only nums[0..2] and nums[n-3..n-1] matter, found by partial selection (O(n log n) with a full sort).
        Space: O(1) — a few extremes.

    Approach (detail):
        1. Changing two elements can always make low = 0 (set both equal to any value), so the answer is the
           smallest possible high score = max - min of what remains.
        2. Changing an element to a value already present never widens the range, so the best move is to
           "remove" two elements from the extremes of the sorted array.
        3. Only three removals matter: the two smallest (range a[n-1]-a[2]), the two largest (a[n-3]-a[0]),
           or the smallest and the largest (a[n-2]-a[1]).
        4. Return the minimum of the three ranges.
*/
/*
    Your solution: sort, then min of the three trims

    Complexity:
        Time:  O(n log n) — the sort dominates.
        Space: O(1) — sorts in place, three integers.

    Approach:
        1. sort(nums); n = nums.size().
        2. a = nums[n-2] - nums[1]  (drop one from each end).
        3. b = nums[n-1] - nums[2]  (drop the two smallest).
        4. c = nums[n-3] - nums[0]  (drop the two largest).
        5. return min(a, min(b, c)).

    Verdict: SUBOPTIMAL — O(n log n) vs O(n) with selection of the extremes; irrelevant at n <= 10^5 (accepted).
        Verified against a brute force (all pairs of changed positions and values) on 20000 random arrays, n <= 7.
*/
class Solution {
    public:
        int minimizeSum(vector<int>& nums) {
            // a1 a2 a3 a4 a5
            // a2 - a1
            // a5 - a1

            // a1->a2
            // a5->a4

            // a4-a2

            // a1-a1
            // a5-a1
            int n = nums.size();
            sort(nums.begin(), nums.end());
            int a = nums[n-2] - nums[1]; // a1->a2, a5->a4, trim 2 đầu
            int b = nums[n-1] - nums[2]; // a1->a3, a2->a3, trim trái
            int c = nums[n-3] - nums[0]; // trim phải
            return min(a, min(b,c));
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
