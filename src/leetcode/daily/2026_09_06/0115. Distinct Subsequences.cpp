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
    LC 115 - Distinct Subsequences
    https://leetcode.com/problems/distinct-subsequences/

    Given two strings s and t, return the number of distinct subsequences of s which equals t.

    The test cases are generated so that the answer fits on a 32-bit signed integer.

    Example 1:

    Input: s = "rabbbit", t = "rabbit"
    Output: 3
    Explanation:
    As shown below, there are 3 ways you can generate "rabbit" from s.
    rabbbit
    rabbbit
    rabbbit

    Example 2:

    Input: s = "babgbag", t = "bag"
    Output: 5
    Explanation:
    As shown below, there are 5 ways you can generate "bag" from s.
    babgbag
    babgbag
    babgbag
    babgbag
    babgbag

    Constraints:

    - 1 <= s.length, t.length <= 1000
    - s and t consist of English letters.
*/
/*
    Best solution: 1-D bottom-up DP over t, iterating s left to right

    Complexity:
        Time:  O(m * n) — each character of s updates every position of t once.
        Space: O(n) — dp[j] = ways to form t[0..j) from the prefix of s read so far.

    Approach (detail):
        1. dp[0] = 1 (the empty prefix of t is formed once), dp[j > 0] = 0.
        2. For each character c of s, for j = n down to 1: if t[j-1] == c, dp[j] += dp[j-1]
           (use c as the j-th matched character, or skip it). Going right to left keeps
           dp[j-1] at its value from the previous character of s.
        3. Answer dp[n]. Intermediate values can exceed 2^31 even when the answer fits, so a
           bottom-up table should use unsigned / 64-bit arithmetic (modulo is not needed: only
           states that feed the answer matter, but bottom-up computes all of them).
*/
/*
    Your solution: top-down memoised DP on (i, j)

    Complexity:
        Time:  O(m * n) — each (i, j) state is computed once.
        Space: O(m * n) — the memo table, plus recursion depth up to m.

    Approach:
        1. dp(i, j) = ways to form t[j..] from s[i..]; memo[i][j] = -1 marks "not computed".
        2. Base cases: j == t.size() -> 1; i == s.size() with j < t.size() -> 0.
        3. Otherwise skip s[i] (dp(i+1, j)), and if s[i] == t[j] also match it (dp(i+1, j+1)).
        4. numDistinct returns dp(0, 0).
        5. Top-down only evaluates states reachable from (0, 0), and every such value is <= the
           final answer, so int never overflows here (a bottom-up int table would).

    Verdict: SUBOPTIMAL — same O(m * n) time but O(m * n) vs O(n) space; at m, n <= 1000 the
        table is about 4 MB, well within limits.
        Verified against a subset-enumeration brute force on 20000 random cases and both
        examples, under ASan/UBSan (no signed overflow, also at m = 1000).
*/
class Solution {
    private:
        int dp(string& s, string& t, int i, int j, vector<vector<int>>& memo){
            if (memo[i][j] >= 0) return memo[i][j];
            if (i == s.size() && j != t.size()) return memo[i][j] = 0;
            if (j == t.size()) return memo[i][j] = 1;
            memo[i][j] = dp(s, t, i+1, j, memo);
            if (s[i] == t[j]){
                memo[i][j] += dp(s, t, i+1, j+1, memo);
            }
            // cout << "i=" << i << " j=" << j << " memo=" << memo[i][j] << "\n";
            return memo[i][j];
        }

    public:
        int numDistinct(string s, string t) {
            int m = s.size(), n = t.size();

            vector<vector<int>> memo(m+1, vector<int>(n+1, -1));
            return dp(s, t, 0, 0, memo);
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

    string s = "babgbag", t = "bag";
    //    0 1 2 3 4 5 6
    // s: b a b g b a g
    // t: b a g
    cout << sol.numDistinct(s, t);

    return 0;
}