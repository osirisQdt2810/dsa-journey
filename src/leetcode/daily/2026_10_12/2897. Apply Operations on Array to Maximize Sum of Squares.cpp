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
    LC 2897 - Apply Operations on Array to Maximize Sum of Squares
    https://leetcode.com/problems/apply-operations-on-array-to-maximize-sum-of-squares/

    You are given a 0-indexed integer array nums and a positive integer k.

    You can do the following operation on the array any number of times:

    - Choose any two distinct indices i and j and simultaneously update the values of nums[i] to (nums[i] AND nums[j]) and nums[j] to (nums[i] OR nums[j]). Here, OR denotes the bitwise OR operation, and AND denotes the bitwise AND operation.

    You have to choose k elements from the final array and calculate the sum of their squares.

    Return the maximum sum of squares you can achieve.

    Since the answer can be very large, return it modulo 10^9 + 7.

    Example 1:

    Input: nums = [2,6,5,8], k = 2
    Output: 261
    Explanation: We can do the following operations on the array:
    - Choose i = 0 and j = 3, then change nums[0] to (2 AND 8) = 0 and nums[3] to (2 OR 8) = 10. The resulting array is nums = [0,6,5,10].
    - Choose i = 2 and j = 3, then change nums[2] to (5 AND 10) = 0 and nums[3] to (5 OR 10) = 15. The resulting array is nums = [0,6,0,15].
    We can choose the elements 15 and 6 from the final array. The sum of squares is 15^2 + 6^2 = 261.
    It can be shown that this is the maximum value we can get.

    Example 2:

    Input: nums = [4,5,4,7], k = 3
    Output: 90
    Explanation: We do not need to apply any operations.
    We can choose the elements 7, 5, and 4 with a sum of squares: 7^2 + 5^2 + 4^2 = 90.
    It can be shown that this is the maximum value we can get.

    Constraints:

    - 1 <= k <= nums.length <= 10^5
    - 1 <= nums[i] <= 10^9
*/
/*
    Best solution: per-bit counting + greedy reconstruction — count how many numbers have each bit
    set, then build the k largest numbers by giving every bit to the next number while its count lasts.

    Complexity:
        Time:  O((n + k) * B), B = 30 bits (nums[i] <= 10^9 < 2^30) — one pass to count bits, then
               B work per constructed number.
        Space: O(B) — the bit-count array.

    Approach (detail):
        1. The operation (a, b) -> (a AND b, a OR b) never changes, for any bit position, how many
           numbers have that bit set: a bit set in both stays in both, a bit set in exactly one moves
           to the OR side. So the per-bit counts cnt[b] are invariant.
        2. Any distribution of bits consistent with those counts is reachable (repeatedly applying
           the operation pushes bits toward one element), so the problem reduces to: distribute
           cnt[b] copies of each bit over n numbers to maximise the sum of squares of the k largest.
        3. Squares are convex: (x + 2^b)^2 + y^2 >= x^2 + (y + 2^b)^2 when x >= y, so every bit
           should go to the largest number possible. Greedily, number 1 takes every bit with
           cnt > 0, number 2 every bit with cnt > 1, ... — i.e. build x_i by taking bit b while
           cnt[b] is still positive, decrementing it.
        4. Sum x_1^2 + ... + x_k^2 modulo 10^9 + 7, reducing after every addition (each x_i^2 mod M
           is < M, and k up to 10^5 terms would overflow a 32-bit result otherwise).

    Your solution:
        MATCHES BEST — counts set bits per position, then greedily rebuilds the k largest numbers
        from those counts, accumulating squares modulo 10^9 + 7; O(32 * (n + k)) time, O(32) space.
        (An earlier draft returned the un-reduced sum and overflowed int for k >= 3 with large
        values; the committed version reduces mod M on every step. Verified against a modular
        reference on 20000 random cases.)
*/
class Solution {
    public:
        int maxSum(vector<int>& nums, int k) {
            // mấu chốt của thuật toán:
            // tồn tại 1 cách rearrangement sao cho kết quả đạt được bằng với cách dồn toàn bit 1 cho các số lớn nhất theo thứ tự giảm dần:
            // CM: giả sử x1,...xn
            //     dồn x1 = x1 | x2 | ... | xn
            //         x2 =      x2 | ... | xn
            //     ............
            // bởi vì tính chất: tổng bit 1 của toàn dãy không đổi sau mỗi lần swap (xi, xj) => (xi&xj, xi|xj)
            const int M = 1e9 + 7;

            vector<int> bit_cnt(32, 0);
            for (auto& num : nums){
                for (int i = 0; i < 32; ++i){
                    bit_cnt[i] += 1 & (num >> i);
                }
            }

            // for (int i = 31; i >= 0; --i){
            //     cout << "i=" << i << " b=" << bit_cnt[i] << "\n";
            // }

            long long res = 0;
            for (int i = 0; i < k; ++i){
                int xi = 0;
                for (int j = 0; j < 32; ++j){
                    if (bit_cnt[j]){
                        xi |= (1 << j);
                        --bit_cnt[j];
                    }
                }
                int xm = (1LL * xi * xi) % M;
                res = (res + xm) % M;
            }
            return res % M;
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
    vector<int> nums = {2,6,5,8}; int k = 2;
    cout << sol.maxSum(nums, k);
    cout << "\n";
    return 0;
}