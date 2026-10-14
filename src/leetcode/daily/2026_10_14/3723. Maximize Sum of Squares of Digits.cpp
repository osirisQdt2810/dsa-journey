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
    LC 3723 - Maximize Sum of Squares of Digits
    https://leetcode.com/problems/maximize-sum-of-squares-of-digits/

    You are given two positive integers num and sum.

    A positive integer n is good if it satisfies both of the following:

    - The number of digits in n is exactly num.
    - The sum of digits in n is exactly sum.

    The score of a good integer n is the sum of the squares of digits in n.

    Return a string denoting the good integer n that achieves the maximum score. If there are multiple possible integers, return the maximum ​​​​​​​one. If no such integer exists, return an empty string.

    Example 1:

    Input: num = 2, sum = 3
    Output: "30"
    Explanation:
    There are 3 good integers: 12, 21, and 30.
    - The score of 12 is 1^2 + 2^2 = 5.
    - The score of 21 is 2^2 + 1^2 = 5.
    - The score of 30 is 3^2 + 0^2 = 9.
    The maximum score is 9, which is achieved by the good integer 30. Therefore, the answer is "30".

    Example 2:

    Input: num = 2, sum = 17
    Output: "98"
    Explanation:
    There are 2 good integers: 89 and 98.
    - The score of 89 is 8^2 + 9^2 = 145.
    - The score of 98 is 9^2 + 8^2 = 145.
    The maximum score is 145. The maximum good integer that achieves this score is 98. Therefore, the answer is "98".

    Example 3:

    Input: num = 1, sum = 10
    Output: ""
    Explanation:
    There are no integers that have exactly 1 digit and whose digits sum to 10. Therefore, the answer is "".

    Constraints:

    - 1 <= num <= 2 * 10^5
    - 1 <= sum <= 2 * 10^6
*/
/*
    Best solution: greedy construction — as many 9s as possible, then the remainder digit, then 0s.

    Complexity:
        Time:  O(num) — linear in the length of the returned string, which is a lower bound.
        Space: O(num) — the output string itself.

    Approach (detail):
        1. A num-digit number has digit sum at most 9 * num; if sum > 9 * num there is no good integer
           -> return "". (sum >= 1 guarantees the leading digit can be non-zero.)
        2. Squares are convex: moving one unit from a smaller digit a to a larger digit b >= a gives
           (b+1)^2 + (a-1)^2 > b^2 + a^2. So any digit multiset with two digits strictly between 0 and
           9 can be improved; the optimum has every digit 9 or 0 except at most one, i.e. exactly
           k = sum / 9 nines, one digit q = sum % 9, and num - k - 1 zeros. That multiset is unique.
        3. Among numbers with that unique multiset (same score), the largest places digits in
           non-increasing order: "9"*k + q + "0"*(num-k-1). When k == num (q must then be 0) the
           answer is "9"*num.
        4. The leading digit is 9 when k >= 1, otherwise q = sum >= 1, so the result never has a
           leading zero.

    Your solution:
        MATCHES BEST — exactly the greedy above: k = sum / 9 nines, the remainder q, then zeros,
        with "" when sum > 9 * num; O(num) time and space. Verified exhaustively against brute force
        for num <= 5, sum <= 50.
*/
class Solution {
    public:
        string maxSumOfSquares(int num, int sum) {
            int k = sum / 9;
            int q = sum % 9;
            if (k == num && q == 0) return string(k, '9');
            if (k + 1 > num) return "";

            return string(k, '9') + to_string(q) + string(num - k - 1, '0');
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