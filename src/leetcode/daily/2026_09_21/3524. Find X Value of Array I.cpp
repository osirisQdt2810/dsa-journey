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
    LC 3524 - Find X Value of Array I
    https://leetcode.com/problems/find-x-value-of-array-i/

    You are given an array of positive integers nums, and a positive integer k.

    You are allowed to perform an operation once on nums, where in each operation you can remove any non-overlapping prefix and suffix from nums such that nums remains non-empty.

    You need to find the x-value of nums, which is the number of ways to perform this operation so that the product of the remaining elements leaves a remainder of x when divided by k.

    Return an array result of size k where result[x] is the x-value of nums for 0 <= x <= k - 1.

    A prefix of an array is a subarray that starts from the beginning of the array and extends to any point within it.

    A suffix of an array is a subarray that starts at any point within the array and extends to the end of the array.

    Note that the prefix and suffix to be chosen for the operation can be empty.

    Example 1:

    Input: nums = [1,2,3,4,5], k = 3
    Output: [9,2,4]
    Explanation:
    - For x = 0, the possible operations include all possible ways to remove non-overlapping prefix/suffix that do not remove nums[2] == 3.
    - For x = 1, the possible operations are:
      - Remove the empty prefix and the suffix [2, 3, 4, 5]. nums becomes [1].
      - Remove the prefix [1, 2, 3] and the suffix [5]. nums becomes [4].
    - For x = 2, the possible operations are:
      - Remove the empty prefix and the suffix [3, 4, 5]. nums becomes [1, 2].
      - Remove the prefix [1] and the suffix [3, 4, 5]. nums becomes [2].
      - Remove the prefix [1, 2, 3] and the empty suffix. nums becomes [4, 5].
      - Remove the prefix [1, 2, 3, 4] and the empty suffix. nums becomes [5].

    Example 2:

    Input: nums = [1,2,4,8,16,32], k = 4
    Output: [18,1,2,0]
    Explanation:
    - For x = 0, the only operations that do not result in x = 0 are:
      - Remove the empty prefix and the suffix [4, 8, 16, 32]. nums becomes [1, 2].
      - Remove the empty prefix and the suffix [2, 4, 8, 16, 32]. nums becomes [1].
      - Remove the prefix [1] and the suffix [4, 8, 16, 32]. nums becomes [2].
    - For x = 1, the only possible operation is:
      - Remove the empty prefix and the suffix [2, 4, 8, 16, 32]. nums becomes [1].
    - For x = 2, the possible operations are:
      - Remove the empty prefix and the suffix [4, 8, 16, 32]. nums becomes [1, 2].
      - Remove the prefix [1] and the suffix [4, 8, 16, 32]. nums becomes [2].
    - For x = 3, there is no possible way to perform the operation.

    Example 3:

    Input: nums = [1,1,2,1,1], k = 2
    Output: [9,6]

    Constraints:

    - 1 <= nums[i] <= 10^9
    - 1 <= nums.length <= 10^5
    - 1 <= k <= 5
*/
/*
    Best solution: DP over the residue of subarray products ending at each index

    Complexity:
        Time:  O(n * k) — k residues propagated per index.
        Space: O(k) — only the previous index's counts are needed.

    Approach (detail):
        1. Removing a prefix and a suffix leaves a non-empty subarray, so x-value counts subarrays
           whose product is congruent to x mod k.
        2. Let cnt[r] = number of subarrays ending at i - 1 with product ≡ r (mod k). For index i
           with v = nums[i] mod k: the new counts are new[(r * v) % k] += cnt[r] for every r, plus
           new[v] += 1 for the subarray [i, i].
        3. Add the new counts to the answer and roll cnt = new; forward propagation needs no
           modular inverse.
*/
/*
    Your solution: forward residue propagation with a rolling row (forward_propagation)

    Complexity:
        Time:  O(n * k) — each index pushes k residues forward once.
        Space: O(k) — only prev_row / curr_row.

    Approach:
        1. curr_row starts at 0 and counts the single-element subarray: curr_row[nums[i] % k]++.
        2. For i > 0, every residue k0 of a subarray ending at i - 1 becomes
           (nums[i] * k0) % k: curr_row[1LL * nums[i] * k0 % k] += prev_row[k0].
        3. Add curr_row into ans, then prev_row = curr_row.
        Also in the file (not called): backward_propagation — the earlier O(n * k^2) / O(n * k)
        version that searches the modular preimages with inv_mod_k; mul is unused.

    Verdict: MATCHES BEST — the O(n * k) forward DP with O(k) memory. Verified against an O(n^2)
        brute force on 20000 random arrays.
*/
class Solution {
    public:
        vector<long long> resultArray(vector<int>& nums, int k) {
            using ll = long long;
            int n = nums.size();
            vector<ll> ans(k, 0);

            auto backward_propagation = [&]() -> void {
                vector<vector<ll>> dp(n, vector<ll>(k, 0));

                auto inv_mod_k = [&](int num, int t) -> vector<int> {
                    // find X: X * num = t (mod k)
                    // return empty if X does not exist
                    vector<int> res;
                    for (int i = 0; i < k; ++i){
                        if (1LL * num * i % k == t){
                            res.push_back(i);
                        }
                    }
                    return res;
                };

                for (int i = 0; i < n; ++i){
                    for (int k0 = 0; k0 < k; ++k0){
                        int local = (nums[i] % k == k0);
                        if (i > 0){
                            auto t_list = inv_mod_k(nums[i], k0);
                            for (auto t : t_list){
                                local += dp[i-1][t];
                            }
                        }
                        ans[k0] += (dp[i][k0] = local);
                    }
                }
            };

            auto mul = [&](int a, int b){
                return 1LL * a * b % k;
            };

            auto forward_propagation = [&]() -> void {
                vector<ll> prev_row(k, 0);
                for (int i = 0; i < n; ++i){
                    vector<ll> curr_row(k, 0);
                    curr_row[nums[i] % k]++;
                    if (i > 0){
                        for (int k0 = 0; k0 < k; ++k0){
                            curr_row[1LL * nums[i] * k0 % k] += prev_row[k0];
                        }
                    }

                    for (int k0 = 0; k0 < k; ++k0){
                        ans[k0] += curr_row[k0];
                    }
                    prev_row = curr_row;
                }
            };

            forward_propagation();

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