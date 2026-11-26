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
    LC 3411 - Maximum Subarray With Equal Products
    https://leetcode.com/problems/maximum-subarray-with-equal-products/
    @status: self
    @created: 2026-11-25 15:30

    You are given an array of positive integers nums.

    An array arr is called product equivalent if prod(arr) == lcm(arr) * gcd(arr), where:

    - prod(arr) is the product of all elements of arr.
    - gcd(arr) is the GCD of all elements of arr.
    - lcm(arr) is the LCM of all elements of arr.

    Return the length of the longest product equivalent subarray of nums.

    Example 1:

    Input: nums = [1,2,1,2,1,1,1]
    Output: 5
    Explanation:
    The longest product equivalent subarray is [1, 2, 1, 1, 1], where prod([1, 2, 1, 1, 1]) = 2, gcd([1, 2, 1, 1, 1]) = 1, and lcm([1, 2, 1, 1, 1]) = 2.

    Example 2:

    Input: nums = [2,3,4,5,6]
    Output: 3
    Explanation:
    The longest product equivalent subarray is [3, 4, 5].

    Example 3:

    Input: nums = [1,2,3,1,4,5,1]
    Output: 5

    Constraints:

    - 2 <= nums.length <= 100
    - 1 <= nums[i] <= 10
*/
/*
    Best solution: extend every start, tracking exact prime-exponent sums for 2, 3, 5, 7

    Complexity:
        Time:  O(n^2 * 4) — each start extends to the right, four primes updated per step.
        Space: O(1) — per-prime sum, min and max exponent.

    Approach (detail):
        1. Since nums[i] <= 10, only primes 2, 3, 5, 7 appear.
        2. prod = gcd * lcm holds iff for every prime p: (sum of exponents of p) = (min exponent) + (max exponent).
        3. For each start i, extend j and keep, per prime, the sum, min and max exponent of nums[i..j].
        4. If all four primes satisfy the equality, the window is product equivalent: res = max(res, j-i+1).
        5. This avoids big products (up to 10^100) and any modular comparison.
*/
/*
    Your solution: brute force over all subarrays, product modulo 1e9+7 compared with gcd * lcm

    Complexity:
        Time:  O(n^2 log V) — all start/end pairs, one gcd and lcm update per step.
        Space: O(1) — three running values.

    Approach:
        1. For each start i, set prod = gcd = lcm = nums[i].
        2. For each j > i: prod = prod * nums[j] % M; gcd = _gcd(gcd, nums[j]); lcm = _lcm(lcm, nums[j]).
        3. If prod == gcd * lcm, a subarray [i..j] is product equivalent: res = max(res, j-i+1).
        4. Return res (initialised to 1, the single-element case).

    Verdict: SUBOPTIMAL — same O(n^2) shape but compares prod % M with the exact gcd * lcm (<= 25200), so a
        false positive is theoretically possible when the true product exceeds M; exact exponent check avoids it.
        Verified against brute force (exact big-product / prime-exponent check) on 20000 small arrays and
        20000 arrays up to n = 100, no mismatch.
*/
class Solution {
    public:
        int maxLength(vector<int>& nums) {
            using ll = long long;
            int n = nums.size();
            int res = 1;
            auto _gcd = [&](ll a, ll b){
                // cout << "a= " << a << " b=" << b << "\n";
                while (b){
                    ll r = b;
                    b = a % b;
                    a = r;
                }
                // cout << "gcd=" << a << "\n";
                return a;
            };

            auto _lcm = [&](ll a, ll b){
                return 1LL * a * b / _gcd(a, b);
            };

            int M = 1e9 + 7;

            for (int i = 0; i < n; ++i){
                ll prod = nums[i];
                ll gcd = nums[i];
                ll lcm = nums[i];
                for (int j = i+1; j < n; ++j){
                    prod = (prod * nums[j]) % M;
                    gcd = _gcd(gcd, nums[j]) % M;
                    lcm = _lcm(lcm, nums[j]) % M;
                    // lcm = 1;
                    if (prod == gcd * lcm){
                        res = max(res, j-i+1);
                    }
                }
            }
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
    // test-lc:begin
    // test-lc:end
    return 0;
}
