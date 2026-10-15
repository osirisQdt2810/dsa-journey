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
    LC 3871 - Count Commas in Range II
    https://leetcode.com/problems/count-commas-in-range-ii/

    You are given an integer n.

    Return the total number of commas used when writing all integers from [1, n] (inclusive) in standard number formatting.

    In standard formatting:

    - A comma is inserted after every three digits from the right.
    - Numbers with fewer than 4 digits contain no commas.

    Example 1:

    Input: n = 1002
    Output: 3
    Explanation:
    The numbers "1,000", "1,001", and "1,002" each contain one comma, giving a total of 3.

    Example 2:

    Input: n = 998
    Output: 0
    Explanation:
    ​​​​​​​All numbers from 1 to 998 have fewer than four digits. Therefore, no commas are used.

    Constraints:

    - 1 <= n <= 10^15
*/
/*
    Best solution: count by thresholds — every number >= 10^3 has at least one comma, every number
    >= 10^6 has a second one, ..., so answer = sum over p = 10^3, 10^6, ... (p <= n) of (n - p + 1).

    Complexity:
        Time:  O(log_1000 n) — at most 5 thresholds for n <= 10^15.
        Space: O(1).

    Approach (detail):
        1. A number with d digits has (d - 1) / 3 commas, i.e. one comma for each threshold
           10^3, 10^6, 10^9, ... that it reaches.
        2. Swap the order of summation: instead of counting commas per number, count for each
           threshold p how many numbers in [1, n] reach it — that is n - p + 1 when p <= n.
        3. Sum over the thresholds with integer arithmetic only (p *= 1000 while p <= n); no
           floating point is needed.
        Equivalent full-block form: numbers in [10^(3c), 10^(3c+3)) contribute c commas each, and
        the last, partial block [10^(3K), n] contributes K * (n - 10^(3K) + 1).

    Your solution:
        MATCHES BEST — the full-block form: K = floor(log10 n) / 3 commas for the partial top block
        (n - 10^(3K) + 1 numbers), plus k * (1000^(k+1) - 1000^k) for every complete block k < K;
        O(log n) time, O(1) space. It uses log10l / pow, which is exact here because n <= 10^15 <
        2^53, but an integer-only loop would be safer for larger ranges. Verified exhaustively for
        n <= 2 * 10^6 and on 200000 random n up to 10^15 (including every power of 10).
*/
class Solution {
    public:
        long long countCommas(long long n) {
            int logn = log10l(n);
            int maxk = logn / 3;
            long long ans = 1LL * (n - pow(10, maxk * 3) + 1) * maxk;
            long long pc = 1;
            // cout << maxk << "\n";
            for (int k = 0; k < maxk; ++k){
                long long pn = pc * 1000;
                ans += 1LL * k * (pn - pc);
                pc = pn;
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

    int n = 1004590;
    cout << sol.countCommas(n);
    return 0;
}