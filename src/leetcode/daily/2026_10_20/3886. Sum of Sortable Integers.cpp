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
    LC 3886 - Sum of Sortable Integers
    https://leetcode.com/problems/sum-of-sortable-integers/

    You are given an integer array nums of length n.

    An integer k is called sortable if k divides n and you can sort nums in non-decreasing order by sequentially performing the following operations:

    - Partition nums into consecutive subarrays of length k.
    - Cyclically rotate each subarray independently any number of times to the left or to the right.

    Return an integer denoting the sum of all possible sortable integers k.

    Example 1:

    Input: nums = [3,1,2]
    Output: 3
    Explanation:​​​​​​​
    - For n = 3, possible divisors are 1 and 3.
    - For k = 1: each subarray has one element. No rotation can sort the array.
    - For k = 3: the single subarray [3, 1, 2] can be rotated once to produce [1, 2, 3], which is sorted.
    - Only k = 3 is sortable. Hence, the answer is 3.

    Example 2:

    Input: nums = [7,6,5]
    Output: 0
    Explanation:
    - For n = 3, possible divisors are 1 and 3.
    - For k = 1: each subarray has one element. No rotation can sort the array.
    - For k = 3: the single subarray [7, 6, 5] cannot be rotated into non-decreasing order.
    - No k is sortable. Hence, the answer is 0.

    Example 3:

    Input: nums = [5,8]
    Output: 3
    Explanation:​​​​​​​
    - For n = 2, possible divisors are 1 and 2.
    - Since [5, 8] is already sorted, every divisor is sortable. Hence, the answer is 1 + 2 = 3.

    Constraints:

    - 1 <= n == nums.length <= 10^5
    - 1 <= nums[i] <= 10^5
*/
/*
    Best solution: prefix count of descents + O(1) per block check

    Complexity:
        Time:  O(n + sigma(n)) = O(n log log n) — O(n) precomputation, then n/k blocks per divisor
               k, and sum over divisors of n/k is sigma(n) <= ~4.5n for n <= 10^5.
        Space: O(n) — the descent prefix array.

    Approach (detail):
        1. A block can be rotated into sorted order iff it is a rotation of a non-decreasing run:
           it has at most one descent (a[i] < a[i-1]) inside it, and with exactly one descent its
           last element must be <= its first (the wrap-around point is not a second descent).
        2. Rotations never move values between blocks, so the whole array becomes sorted iff
           every block is sortable on its own AND max(block j) <= min(block j+1) for all j.
        3. Precompute desc[i] = number of descents among positions 1..i. For a block [s, e] the
           count inside is desc[e] - desc[s]; the block's min/max are then O(1):
             - 0 descents: min = a[s], max = a[e];
             - 1 descent at p (located via a second prefix of descent positions, or by storing
               the descent index): min = a[p], max = a[p-1] — and require a[e] <= a[s].
        4. For every divisor k of n, walk the n/k blocks with these O(1) checks and the chain
           condition; add k to the answer when all pass.
*/
/*
    Your solution: per-divisor full scan of every block (descent count + min/max)

    Complexity:
        Time:  O(n * d(n)) — for each of the d(n) <= 128 divisors, sortable() scans all n
               elements once; <= ~1.3 * 10^7 steps at n = 10^5.
        Space: O(d(n)) — the divisor list (temp_group is cleared but never filled).

    Approach:
        1. Collect all divisors of n in `divisors` by trial division up to sqrt(n).
        2. sortable(nums, start, end) scans the block once: n_groups counts non-decreasing runs
           (1 + number of descents) and minmax tracks the block's min and max. More than 2 runs
           returns null = {0, -1} (min > max marks invalid); exactly 2 runs needs
           nums[end] <= nums[start]; otherwise minmax is returned.
        3. For each divisor d: check the first block, then walk the following blocks i = d, 2d, ...,
           failing as soon as the block is invalid (emin2 > emax2) or the previous block's
           emax > the current block's emin2.
        4. ans += d when every block passed.

    Verdict: SUBOPTIMAL — correct, but O(n * d(n)) vs O(n log log n): each block is rescanned
        element by element instead of answered in O(1) from a descent prefix. Still well within
        limits at n <= 10^5. Verified against a brute force (match every block to its slice of the
        sorted array under all rotations) on 200000 random arrays, n <= 12.
*/
class Solution {
    public:
        int sortableIntegers(vector<int>& nums) {
            // 7 6 5
            // 6 5 7
            // 5 7 6

            // (a1 a2 ... ak) (b1 b2 ... bk) ... (x1 x2 ... xk)
            // (a1 a2 a3 ... ak)
            // (a2 a3 ... ak a1)
            // (a3 a4 ... ak a1 a2)
            int n = nums.size();
            vector<int> divisors;
            for (int i = 1; i * i <= n; ++i){
                if (n % i == 0){
                    divisors.push_back(i);
                    if (i * i < n){
                        divisors.push_back(n / i);
                    }
                }
            }

            using ii = pair<int, int>;

            vector<int> temp_group;
            auto sortable = [&](const vector<int>& nums, int start, int end) -> ii {
                temp_group.clear();
                int n_groups = 1;
                ii null = {0, -1};
                ii minmax = {nums[start], nums[start]};
                for (int i = start+1; i <= end; ++i){
                    if (nums[i] < nums[i-1]){
                        n_groups++;

                    }
                    minmax.first = min(minmax.first, nums[i]);
                    minmax.second = max(minmax.second, nums[i]);
                }
                if (n_groups > 2) return null;   // invalid
                if (n_groups == 2) return (nums[end] <= nums[start]) ? minmax : null;
                return minmax;
            };

            int ans = 0;
            for (auto d : divisors){
                auto [emin, emax] = sortable(nums, 0, d-1);
                if (emin > emax) continue;

                bool is_valid = true;
                for (int i = d; i < n; i += d){
                    auto [emin2, emax2] = sortable(nums, i, i+d-1);
                    if (emax > emin2 || emin2 > emax2){
                        is_valid = false;
                        break;
                    }
                    emin = emin2;
                    emax = emax2;
                }
                ans += d * int(is_valid);
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