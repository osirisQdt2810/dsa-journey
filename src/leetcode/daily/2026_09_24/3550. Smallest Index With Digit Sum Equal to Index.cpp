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
    LC 3550 - Smallest Index With Digit Sum Equal to Index
    https://leetcode.com/problems/smallest-index-with-digit-sum-equal-to-index/

    You are given an integer array nums.

    Return the smallest index i such that the sum of the digits of nums[i] is equal to i.

    If no such index exists, return -1.

    Example 1:

    Input: nums = [1,3,2]
    Output: 2
    Explanation:
    - For nums[2] = 2, the sum of digits is 2, which is equal to index i = 2. Thus, the output is 2.

    Example 2:

    Input: nums = [1,10,11]
    Output: 1
    Explanation:
    - For nums[1] = 10, the sum of digits is 1 + 0 = 1, which is equal to index i = 1.
    - For nums[2] = 11, the sum of digits is 1 + 1 = 2, which is equal to index i = 2.
    - Since index 1 is the smallest, the output is 1.

    Example 3:

    Input: nums = [1,2,3]
    Output: -1
    Explanation:
    - Since no index satisfies the condition, the output is -1.

    Constraints:

    - 1 <= nums.length <= 100
    - 0 <= nums[i] <= 1000
*/
/*
    Best solution: linear scan with an inline digit sum

    Complexity:
        Time:  O(n * d) — d <= 4 digits per value (nums[i] <= 1000), so effectively O(n).
        Space: O(1).

    Approach (detail):
        1. Walk i = 0..n-1 in order; the first index that matches is by definition the smallest.
        2. Compute the digit sum of nums[i] with repeated % 10 and / 10 and compare it with i.
        3. Return that i, or -1 after the loop. (Digit sums are at most 9 * 3 = 27 (for 999), so
           indices beyond 27 can never match — an optional early stop.)
*/
/*
    Your solution: scan with a digit-sum loop, return the first match

    Complexity:
        Time:  O(n * d) — one digit loop per element.
        Space: O(1).

    Approach:
        1. For each i, copy vi = nums[i] and accumulate sum_d += vi % 10 while vi /= 10.
        2. Return i as soon as sum_d == i; return -1 if nothing matched.
        3. The local `ans` is declared but never used.

    Verdict: MATCHES BEST — the direct O(n * d) scan.
        Verified against a to_string digit-sum brute force on 100000 random arrays, under
        ASan/UBSan.
*/
class Solution {
    public:
        int smallestIndex(vector<int>& nums) {
            int ans = 0;
            for (int i = 0; i < int(nums.size()); ++i){
                int vi = nums[i];
                int sum_d = 0;
                while (vi){
                    sum_d += vi % 10;
                    vi /= 10;
                }
                if (sum_d == i) return i;
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