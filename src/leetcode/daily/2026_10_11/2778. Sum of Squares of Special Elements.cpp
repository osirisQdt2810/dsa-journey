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
    LC 2778 - Sum of Squares of Special Elements
    https://leetcode.com/problems/sum-of-squares-of-special-elements/

    Problem:
        You are given a 1-indexed integer array nums of length n. An element nums[i] is called special
        if i divides n, i.e. n % i == 0. Return the sum of the squares of all special elements of nums.

        Constraints:
            1 <= nums.length == n <= 50
            1 <= nums[i] <= 50

    Examples:
        Input: nums = [1,2,3,4]
        Output: 21
        Explanation: special indices 1, 2, 4 -> 1*1 + 2*2 + 4*4 = 21.

        Input: nums = [2,7,1,19,18,3]
        Output: 63
        Explanation: special indices 1, 2, 3, 6 -> 2*2 + 7*7 + 1*1 + 3*3 = 63.

    Best solution: enumerate divisors of n (direct scan, or pairs i and n/i up to sqrt(n)).

    Complexity:
        Time:  O(n) for the direct scan (O(sqrt n) with divisor pairs) — n <= 50, so both are trivial.
        Space: O(1).

    Approach (detail):
        1. For every 1-indexed i in [1, n], check n % i == 0.
        2. If so add nums[i-1]^2 to the answer (max 50 * 50^2, fits in int).
        3. Optional speed-up: for i <= sqrt(n) with n % i == 0, add both i and n/i (once if equal).

    Your solution:
        MATCHES BEST — direct O(n) divisor scan; the O(sqrt n) variant gives no practical gain at
        n <= 50.
*/

class Solution {
    public:
        int sumOfSquares(vector<int>& nums) {
            int n = nums.size();
            int res = 0;
            for (int i = 1; i <= n; ++i){
                if (n % i == 0){
                    res += nums[i-1] * nums[i-1];
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
    Solution sol;

    return 0;
}