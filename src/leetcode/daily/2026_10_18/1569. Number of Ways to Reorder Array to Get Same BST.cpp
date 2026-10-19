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
    LC 1569 - Number of Ways to Reorder Array to Get Same BST
    https://leetcode.com/problems/number-of-ways-to-reorder-array-to-get-same-bst/

    Given an array nums that represents a permutation of integers from 1 to n. We are going to construct a binary search tree (BST) by inserting the elements of nums in order into an initially empty BST. Find the number of different ways to reorder nums so that the constructed BST is identical to that formed from the original array nums.

    - For example, given nums = [2,1,3], we will have 2 as the root, 1 as a left child, and 3 as a right child. The array [2,3,1] also yields the same BST but [3,2,1] yields a different BST.

    Return the number of ways to reorder nums such that the BST formed is identical to the original BST formed from nums.

    Since the answer may be very large, return it modulo 10^9 + 7.

    Example 1:

    Input: nums = [2,1,3]
    Output: 1
    Explanation: We can reorder nums to be [2,3,1] which will yield the same BST. There are no other ways to reorder nums which will yield the same BST.

    Example 2:

    Input: nums = [3,4,5,1,2]
    Output: 5
    Explanation: The following 5 arrays will yield the same BST:
    [3,1,2,4,5]
    [3,1,4,2,5]
    [3,1,4,5,2]
    [3,4,1,2,5]
    [3,4,1,5,2]

    Example 3:

    Input: nums = [1,2,3]
    Output: 0
    Explanation: There are no other orderings of nums that will yield the same BST.

    Constraints:

    - 1 <= nums.length <= 1000
    - 1 <= nums[i] <= nums.length
    - All integers in nums are distinct.
*/
/*
    Best solution: recursive split around the root + binomial interleavings (Pascal's triangle)

    Complexity:
        Time:  O(n^2) — every level partitions its subarray, and Pascal's table is n x n.
        Space: O(n^2) — the binomial table (O(n) recursion depth).

    Approach (detail):
        1. The first element is the root; the rest split into left (< root) and right (> root),
           each keeping its relative order.
        2. Any reordering that keeps the same BST must keep the root first and keep each side's own
           valid order, but the two sides can be interleaved freely: C(L + R, L) ways.
        3. ways(arr) = C(L + R, L) * ways(left) * ways(right), with ways(empty) = 1.
        4. Precompute C(n, k) mod 10^9 + 7 with Pascal's rule, and return ways(nums) - 1 (the
           original order itself is excluded), taken modulo 10^9 + 7.
*/
/*
    Your solution: recursive left / right split with C(L+R, L) computed from products and a Fermat inverse

    Complexity:
        Time:  O(n^2) — partitions cost O(n) per level; each combination() multiplies O(L+R) terms
               and inverts in O(log MOD).
        Space: O(n^2) worst case — the left / right vectors created along a skewed recursion.

    Approach:
        1. solve(nums): root = nums[0]; push smaller values to left and larger values to right in
           order; return 1 for an empty array.
        2. merge(L, R) = combination(L, L + R) = divisor(...) * inv(dividend(...)): the product of
           the top L factors of (L+R)! times the modular inverse of L!, with inv via fast exponent
           a^(MOD-2) (_inv).
        3. solve returns mul(mul(solve(left), solve(right)), merge(|left|, |right|)).
        4. numOfWays returns solve(nums) - 1.

    Verdict: MATCHES BEST — the same split-and-interleave recursion in O(n^2), with each binomial
        computed directly instead of from a Pascal table (same order of work). One latent edge:
        solve(nums) - 1 is not reduced modulo 10^9 + 7, so it would return -1 if the count were
        ever exactly a multiple of the modulus. Verified against a brute force over all
        permutations for n <= 7 and on the sorted n = 1000 case.
*/
class Solution {
    private:
        static constexpr const int MOD = 1e9 + 7;

        int mul(int a, int b){
            return (1LL * a * b) % MOD;
        }

        int _inv(int a, int m){
            if (m == 0) return 1;
            int d = m / 2;
            int r = _inv(a, d);
            return (m % 2) ? mul(mul(r, r), a) : mul(r, r);
        }

        int inv(int a){
            // ~a^(MOD-2) % MOD
            return _inv(a, MOD-2);
        }

        int dividend(int k, int n){
            // 1*2*...*k
            int res = 1;
            for (int i=1; i<=k; ++i){
                res = mul(res, i);
            }
            return res;
        }

        int divisor(int k, int n){
            // (n-k+1)*(n-k+2)*...*n
            int res = 1;
            for (int i=0; i<k; ++i){
                res = mul(res, n-i);
            }
            return res;
        }

        int combination(int k, int n){
            int numerator = divisor(k, n);
            int denominator = dividend(k, n);
            return mul(numerator, inv(denominator));
        }

        int merge(int L, int R){
            // return C_L_{L+R}
            return combination(L, L+R);
        }

        int solve(vector<int>& nums){
            if (nums.empty()) return 1;

            vector<int> left, right;
            int root = nums[0];
            for (int i = 1; i < (int)nums.size(); ++i){
                if (nums[i] < root) left.push_back(nums[i]);
                if (nums[i] > root) right.push_back(nums[i]);
            }

            return mul(mul(solve(left), solve(right)), merge(left.size(), right.size()));
        }


    public:
        int numOfWays(vector<int>& nums) {
            return solve(nums) - 1;
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