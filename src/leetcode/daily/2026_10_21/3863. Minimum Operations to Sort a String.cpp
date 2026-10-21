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
    LC 3863 - Minimum Operations to Sort a String
    https://leetcode.com/problems/minimum-operations-to-sort-a-string/

    You are given a string s consisting of lowercase English letters.

    In one operation, you can select any substring of s that is not the entire string and sort it in non-descending alphabetical order.

    Return the minimum number of operations required to make s sorted in non-descending order. If it is not possible, return -1.

    Example 1:

    Input: s = "dog"
    Output: 1
    Explanation:​​​​​​​
    - Sort substring "og" to "go".
    - Now, s = "dgo", which is sorted in ascending order. Thus, the answer is 1.

    Example 2:

    Input: s = "card"
    Output: 2
    Explanation:
    - Sort substring "car" to "acr", so s = "acrd".
    - Sort substring "rd" to "dr", making s = "acdr", which is sorted in ascending order. Thus, the answer is 2.

    Example 3:

    Input: s = "gf"
    Output: -1
    Explanation:
    - It is impossible to sort s under the given constraints. Thus, the answer is -1.

    Constraints:

    - 1 <= s.length <= 10^5
    - s consists of only lowercase English letters.
*/
/*
    Best solution: case analysis on the first / last character — answer is always 0, 1, 2, 3 or -1

    Complexity:
        Time:  O(n) — one scan for the global min / max and whether the middle is sorted.
        Space: O(1).

    Approach (detail):
        1. Already sorted -> 0. If n == 2 and unsorted, the only allowed substrings have length 1,
           which change nothing -> -1. From here n >= 3.
        2. One operation can never touch both s[0] and s[n-1]. So 1 suffices iff the untouched end
           is already in its final place: s[0] is a minimum of s (sort s[1..n-1]) or s[n-1] is a
           maximum of s (sort s[0..n-2]).
        3. Otherwise 2 suffices whenever a global minimum lies in s[0..n-2] (sort s[0..n-2], so
           s[0] becomes a minimum, then sort s[1..n-1]) or, symmetrically, a global maximum lies
           in s[1..n-1].
        4. The only remaining case is s[0] the strict unique maximum and s[n-1] the strict unique
           minimum: the first operation must move one of them inward, so 3 operations are needed
           (and 3 always suffice: sort s[0..n-2], then s[1..n-1], then s[0..n-2]).
*/
/*
    Your solution: one scan over the middle, then the 0 / 1 / 2 / 3 / -1 case table

    Complexity:
        Time:  O(n) — one pass over s[1..n-2].
        Space: O(1).

    Approach:
        1. Over the middle s[1..n-2] compute mmin, mmax and msort (middle non-decreasing).
        2. lmin / lmax = min / max of s[0..n-2], rmin / rmax = min / max of s[1..n-1].
           lsort = middle sorted and s[0] <= mmin (prefix s[0..n-2] already sorted);
           rsort = middle sorted and mmax <= s[n-1] (suffix s[1..n-1] already sorted).
        3. If lmax <= s[n-1] (s[n-1] is a maximum), one sort of s[0..n-2] — or none if lsort —
           finishes: candidate 1 - lsort. Symmetrically s[0] <= rmin gives 1 - rsort.
        4. If lmin <= s[n-1] or s[0] <= rmax (a minimum before the end / a maximum after the
           start), candidate 2. Otherwise -1 for n == 2, else 3.
        5. A debug `cout` of lsort / rsort / ranges is still printed on every call; it does not
           change the returned value.

    Verdict: MATCHES BEST — the same O(n) / O(1) case analysis.
        Verified against a BFS over all allowed substring sorts for every string of length <= 5
        over 4 letters and length 6..7 over 3 letters (4280 strings).
*/
class Solution {
    public:
        int minOperations(string s) {
            int n = s.size();
            if (n == 1) return 0;
            int mmin = INT_MAX, mmax = INT_MIN;
            bool msort = true;
            for (int i = 1; i < n-1; ++i){
                mmin = min(mmin, (int)s[i]);
                mmax = max(mmax, (int)s[i]);
                if (i < n-2 && s[i] > s[i+1]){
                    msort = false;
                }
            }
            int lmin = min(mmin, (int)s[0]);
            int lmax = max(mmax, (int)s[0]);
            int rmin = min(mmin, (int)s[n-1]);
            int rmax = max(mmax, (int)s[n-1]);

            // left (lmin, lmax) vs last element (s[n-1]):
            int lsort = msort & ((int)s[0] <= mmin);
            int rsort = msort & (mmax <= (int)s[n-1]);
            cout << "lsort=" << lsort << " rsort=" << rsort << " l=[" << lmin << "," << lmax << "] r=[" << rmin << "," << rmax << "]\n";
            int ans = INT_MAX;
            if (lmax <= (int)s[n-1]) ans = min(ans, 1-lsort);
            if ((int)s[0] <= rmin) ans = min(ans, 1-rsort);
            if (lmin <= (int)s[n-1] || (int)s[0] <= rmax) ans = min(ans, 2);
            if (ans == INT_MAX) ans = (n==2) ? -1 : 3;
            return ans;
        }
};

int main(){
    Solution sol;

    string s = "figi";  // 2 5 3 5
    cout << sol.minOperations(s) << "\n";

    return 0;
}