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
    LC 2911 - Minimum Changes to Make K Semi-palindromes
    https://leetcode.com/problems/minimum-changes-to-make-k-semi-palindromes/
    @status: hint
    @time: 50
    @wa: 0
    @created: 2026-11-17 11:05

    Given a string s and an integer k, partition s into k substrings such that the letter changes needed to make each substring a semi-palindrome are minimized.

    Return the minimum number of letter changes required.

    A semi-palindrome is a special type of string that can be divided into palindromes based on a repeating pattern. To check if a string is a semi-palindrome:​

    1. Choose a positive divisor d of the string's length. d can range from 1 up to, but not including, the string's length. For a string of length 1, it does not have a valid divisor as per this definition, since the only divisor is its length, which is not allowed.
    2. For a given divisor d, divide the string into groups where each group contains characters from the string that follow a repeating pattern of length d. Specifically, the first group consists of characters at positions 1, 1 + d, 1 + 2d, and so on; the second group includes characters at positions 2, 2 + d, 2 + 2d, etc.
    3. The string is considered a semi-palindrome if each of these groups forms a palindrome.

    Consider the string "abcabc":

    - The length of "abcabc" is 6. Valid divisors are 1, 2, and 3.
    - For d = 1: The entire string "abcabc" forms one group. Not a palindrome.
    - For d = 2:
      - Group 1 (positions 1, 3, 5): "acb"
      - Group 2 (positions 2, 4, 6): "bac"
      - Neither group forms a palindrome.
    - For d = 3:
      - Group 1 (positions 1, 4): "aa"
      - Group 2 (positions 2, 5): "bb"
      - Group 3 (positions 3, 6): "cc"
      - All groups form palindromes. Therefore, "abcabc" is a semi-palindrome.

    Example 1:

    Input:   s = "abcac", k = 2
    Output:   1
    Explanation:  Divide s into "ab" and "cac". "cac" is already semi-palindrome. Change "ab" to "aa", it becomes semi-palindrome with d = 1.

    Example 2:

    Input:   s = "abcdef", k = 2
    Output:   2
    Explanation:  Divide s into substrings "abc" and "def". Each needs one change to become semi-palindrome.

    Example 3:

    Input:   s = "aabbaa", k = 3
    Output:   0
    Explanation:  Divide s into substrings "aa", "bb" and "aa". All are already semi-palindromes.

    Constraints:

    - 2 <= s.length <= 200
    - 1 <= k <= s.length / 2
    - s contains only lowercase English letters.
*/
/*
    Best solution: per-substring semi-palindrome cost over the divisors + DP over the number of pieces

    Complexity:
        Time:  O(n^3 * D) for the costs (D = max number of divisors of a length, <= 12 for n <= 200)
               plus O(n^2 * k) for the DP; both are tiny for n <= 200.
        Space: O(n^2) for the costs + O(n * k) for the DP.

    Approach (detail):
        1. cost[i][j] = fewest letter changes to make s[i..j] a semi-palindrome. For a piece of length
           len >= 2, try every divisor d of len with 1 <= d < len: split it into d groups (positions
           i+g, i+g+d, ...) and count the mismatching mirrored pairs inside each group. Take the
           minimum over d.
        2. A piece of length 1 has no valid divisor, so it is not allowed (cost infinite); the
           constraint k <= n / 2 guarantees a split into k pieces of length >= 2 always exists.
        3. f[p][l] = fewest changes to split the prefix s[0..p-1] into l valid pieces:
           f[0][0] = 0, f[p][l] = min over q < p of f[q][l-1] + cost[q][p-1].
        4. The answer is f[n][k].
*/
/*
    Your solution: cost table over the divisors, then prefix DP f[p][l]

    Complexity:
        Time:  O(n^3 * D + n^2 * k) — D = number of divisors of a piece length.
        Space: O(n^2 + n * k).

    Approach:
        1. divisors[len] lists d with d | len and 1 <= d < len (empty for len = 1).
        2. changes[i][j] starts at INF; for every piece s[i..j] (j > i) and every d in divisors[len],
           each group g (l = i+g, r = i+g+(len/d-1)*d, moving inwards by d) adds the mismatching pairs;
           changes[i][j] keeps the minimum over d.
        3. f[0][0] = 0; for p = 1..n, l = 1..k, q = 0..p-1: when f[q][l-1] and changes[q][p-1] are both
           finite, f[p][l] = min(f[p][l], f[q][l-1] + changes[q][p-1]).
        4. Return f[n][k].

    Verdict: MATCHES BEST — the standard O(n^3 * D + n^2 * k) solution. Verified: 3/3 official examples and
        a definition-based brute force over every split on 3000 random strings (n <= 10, k <= n / 2),
        with length-1 pieces treated as invalid.
        Note: the DP was fixed with Claude's help (@status: hint).
*/
class Solution {
    public:
        int minimumChanges(string s, int k) {
            int n = s.size();
            const int INF = INT_MAX / 2;

            // divisors[len] = d with d | len and 1 <= d < len (none for len = 1)
            vector<vector<int>> divisors(n + 1);
            for (int len = 2; len <= n; ++len)
                for (int d = 1; d < len; ++d)
                    if (len % d == 0) divisors[len].push_back(d);

            // changes[i][j] = fewest letters to change in s[i..j] to make it a semi-palindrome
            // (INF when the piece has length 1: it has no valid divisor)
            vector<vector<int>> changes(n, vector<int>(n, INF));
            for (int i = 0; i < n; ++i){
                for (int j = i + 1; j < n; ++j){
                    int len = j - i + 1;
                    for (int d : divisors[len]){
                        // group g = positions i+g, i+g+d, ...; the group must read the same both ways
                        int cij = 0;
                        for (int g = 0; g < d; ++g){
                            int l = i + g;
                            int r = i + g + (len / d - 1) * d;
                            while (l < r){
                                cij += s[l] != s[r];
                                l += d;
                                r -= d;
                            }
                        }
                        changes[i][j] = min(changes[i][j], cij);
                    }
                }
            }

            // f[p][l] = fewest changes to split the prefix s[0..p-1] into l valid pieces
            vector<vector<int>> f(n + 1, vector<int>(k + 1, INF));
            f[0][0] = 0;
            for (int p = 1; p <= n; ++p){
                for (int l = 1; l <= k; ++l){
                    for (int q = 0; q < p; ++q){           // last piece is s[q..p-1]
                        if (f[q][l - 1] >= INF || changes[q][p - 1] >= INF) continue;
                        f[p][l] = min(f[p][l], f[q][l - 1] + changes[q][p - 1]);
                    }
                }
            }
            return f[n][k];
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
    int passed = 0, total = 0;
    auto check = [&](int idx, int got, int expected) {
        ++total;
        if (got == expected) {
            ++passed;
            cout << "Example " << idx << ": PASS\n";
        } else {
            cout << "Example " << idx << ": FAIL  got=" << got << "  expected=" << expected << "\n";
        }
    };
    {
        Solution sol;
        string s = "abcac";
        int k = 2;
        check(1, sol.minimumChanges(s, k), 1);
    }
    {
        Solution sol;
        string s = "abcdef";
        int k = 2;
        check(2, sol.minimumChanges(s, k), 2);
    }
    {
        Solution sol;
        string s = "aabbaa";
        int k = 3;
        check(3, sol.minimumChanges(s, k), 0);
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}
