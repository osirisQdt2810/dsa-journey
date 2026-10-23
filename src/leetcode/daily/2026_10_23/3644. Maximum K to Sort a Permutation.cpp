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
    LC 3644 - Maximum K to Sort a Permutation
    https://leetcode.com/problems/maximum-k-to-sort-a-permutation/

    You are given an integer array nums of length n, where nums is a permutation of the numbers in the range [0..n - 1].

    You may swap elements at indices i and j only if nums[i] AND nums[j] == k, where AND denotes the bitwise AND operation and k is a non-negative integer.

    Return the maximum value of k such that the array can be sorted in non-decreasing order using any number of such swaps. If nums is already sorted, return 0.

    Example 1:

    Input: nums = [0,3,2,1]
    Output: 1
    Explanation:
    Choose k = 1. Swapping nums[1] = 3 and nums[3] = 1 is allowed since nums[1] AND nums[3] == 1, resulting in a sorted permutation: [0, 1, 2, 3].

    Example 2:

    Input: nums = [0,1,3,2]
    Output: 2
    Explanation:
    Choose k = 2. Swapping nums[2] = 3 and nums[3] = 2 is allowed since nums[2] AND nums[3] == 2, resulting in a sorted permutation: [0, 1, 2, 3].

    Example 3:

    Input: nums = [3,2,1,0]
    Output: 0
    Explanation:
    Only k = 0 allows sorting since no greater k allows the required swaps where nums[i] AND nums[j] == k.

    Constraints:

    - 1 <= n == nums.length <= 10^5
    - 0 <= nums[i] <= n - 1
    - nums is a permutation of integers from 0 to n - 1.
*/
/*
    Best solution: k = bitwise AND of every misplaced value (0 if already sorted)

    Complexity:
        Time:  O(n) — one pass.
        Space: O(1).

    Approach (detail):
        1. Upper bound: every misplaced value x must take part in at least one swap, and a swap
           of x with y needs x & y == k, so k is a submask of x. Hence k is a submask of the AND
           of all misplaced values, i.e. k <= that AND.
        2. Achievability: let k = AND of the misplaced values. Since nums is a permutation of
           0..n-1 and k <= n-1, the value k exists in the array, and k & x == k for every
           misplaced x (k is a submask of each). So the element k can swap with any misplaced
           element.
        3. Use k as a pivot, like sorting with a buffer slot: walk each cycle of the permutation
           by repeatedly swapping k with the value that belongs where k currently sits (if k is
           already in place, first swap it into the cycle). Every swap involves k and a misplaced
           value, so every swap is legal; this sorts the array.
        4. Sorted input has no misplaced value -> answer 0.
*/
/*
    Your solution: AND over the misplaced values

    Complexity:
        Time:  O(n) — one pass.
        Space: O(1).

    Approach:
        1. res = -1 marks "no misplaced value seen yet".
        2. For each i with nums[i] != i: the first one sets res = nums[i], later ones do
           res &= nums[i].
        3. Return max(res, 0): 0 when the array is already sorted.

    Verdict: MATCHES BEST — the O(n) / O(1) AND-of-misplaced formula.
        Verified against a BFS over legal swaps (for every k) on all permutations with n <= 7
        (5913 permutations).
*/
class Solution {
    public:
        int sortPermutation(vector<int>& nums) {
            int n = nums.size();
            int res = -1;
            for (int i = 0; i < n; ++i){
                if (i != nums[i]){
                    res = (res < 0) ? nums[i] : (res & nums[i]);
                }
            }
            return max(res, 0);
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