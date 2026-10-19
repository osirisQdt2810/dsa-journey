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
    LC 3498 - Reverse Degree of a String
    https://leetcode.com/problems/reverse-degree-of-a-string/

    Given a string s, calculate its reverse degree.

    The reverse degree is calculated as follows:

    1. For each character, multiply its position in the reversed alphabet ('a' = 26, 'b' = 25, ..., 'z' = 1) with its position in the string (1-indexed).
    2. Sum these products for all characters in the string.

    Return the reverse degree of s.

    Example 1:

    Input: s = "abc"
    Output: 148
    Explanation:
     | Letter  | Index in Reversed Alphabet  | Index in String  | Product
     | 'a'  | 26  | 1  | 26
     | 'b'  | 25  | 2  | 50
     | 'c'  | 24  | 3  | 72
    The reversed degree is 26 + 50 + 72 = 148.

    Example 2:

    Input: s = "zaza"
    Output: 160
    Explanation:
     | Letter  | Index in Reversed Alphabet  | Index in String  | Product
     | 'z'  | 1  | 1  | 1
     | 'a'  | 26  | 2  | 52
     | 'z'  | 1  | 3  | 3
     | 'a'  | 26  | 4  | 104
    The reverse degree is 1 + 52 + 3 + 104 = 160.

    Constraints:

    - 1 <= s.length <= 1000
    - s contains only lowercase English letters.
*/
/*
    Best solution: single pass summing position * reversed-alphabet value

    Complexity:
        Time:  O(n) — one pass.
        Space: O(1).

    Approach (detail):
        1. In the reversed alphabet 'a' = 26, ..., 'z' = 1, so a letter c is worth 26 - (c - 'a').
        2. The reverse degree is the sum over positions i (1-indexed) of i * value(s[i]).
*/
/*
    Your solution: one loop accumulating (i + 1) * (26 - (s[i] - 'a'))

    Complexity:
        Time:  O(n).
        Space: O(1).

    Approach:
        1. For each index i, add (i + 1) * (26 - s[i] + 'a') to ans.
        2. Return ans (at most 1000 * 1001 / 2 * 26, fits in int).

    Verdict: MATCHES BEST — the direct O(n) formula. Checked on the problem's examples
        ("abc" -> 148, "zaza" -> 160).
*/
class Solution {
    public:
        int reverseDegree(string s) {
            int ans = 0;
            for (int i = 0; i < (int)s.size(); ++i){
                ans += (i + 1) * (26 - s[i] + 'a');
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

    return 0;
}