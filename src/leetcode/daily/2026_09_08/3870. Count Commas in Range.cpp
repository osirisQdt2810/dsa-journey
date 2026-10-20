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

using namespace std;

/*
    LC 3870 - Count Commas in Range
    https://leetcode.com/problems/count-commas-in-range/

    You are given an integer n.

    Return the total number of commas used when writing all integers from [1, n] (inclusive) in standard number formatting.

    In standard formatting:

    - A comma is inserted after every three digits from the right.
    - Numbers with fewer than 4 digits contain no commas.

    Example 1:

    Input: n = 1002006 => 6
    Output: 3
    Explanation:
    The numbers "1,000", "1,001", and "1,002" each contain one comma, giving a total of 3.

    Example 2:

    Input: n = 998
    Output: 0
    Explanation:
    All numbers from 1 to 998 have fewer than four digits. Therefore, no commas are used.

    Constraints:

    - 1 <= n <= 10^5
*/
/*
    Best solution: count per comma-group, closed form

    Complexity:
        Time:  O(log n) — one term per group of three digits (O(1) for n <= 10^5).
        Space: O(1).

    Approach (detail):
        1. A number with d digits has (d - 1) / 3 commas, so the count only changes at
           10^3, 10^6, 10^9, ...
        2. Every number >= 10^(3j) contributes at least j commas; summing "one extra comma per
           threshold passed" gives answer = sum over j >= 1 of max(0, n - 10^(3j) + 1).
        3. Under n <= 10^5 this is simply max(0, n - 999).
*/
/*
    Your solution: digit-group bands summed up to n

    Complexity:
        Time:  O(log n) — the loop runs once per complete band of three digits.
        Space: O(1).

    Approach:
        1. ln = floor(log10f(n)) is digits - 1; ln3 rounds it down to a multiple of 3, the
           exponent of the band [10^ln3, ...) that contains n.
        2. For each full band [10^k, 10^(k+3)) below it, add (k / 3) * (10^(k+3) - 10^k):
           every number there has k / 3 commas.
        3. Finally add (k / 3) * (n - 10^k + 1) for the partial band that ends at n.

    Verdict: MATCHES BEST — the same band counting, O(log n) time, O(1) space. The digit count
        comes from the float log10f, which is exact at 10^3..10^5 here; an integer digit count
        would not depend on float rounding.
        Verified exhaustively for every n in [1, 10^5] against a per-number comma count.
*/
class Solution {
    public:
        int countCommas(int n) {
            // 0 -> 10^3 - 1: 0
            // 10^3 -> 10^6 - 1: 1*(10^6 - 10^3)
            // ...
            // 10^(3*k) -> X: X - 10^(3*k) + 1
            int ln = log10f(n);
            int ln3 = (ln / 3) * 3;
            int ans = 0;
            long long pow = 1;
            int k = 0;
            for (; k + 3 <= ln3; k += 3){
                long long npow = pow * 1000;
                ans += (k / 3) * (npow - pow);
                pow = npow;
            }
            // 10^k => n
            ans += (k / 3) * (n - pow + 1);
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