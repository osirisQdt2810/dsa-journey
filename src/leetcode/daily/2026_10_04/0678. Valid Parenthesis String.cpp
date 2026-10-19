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
    LC 678 - Valid Parenthesis String
    https://leetcode.com/problems/valid-parenthesis-string/

    Given a string s containing only three types of characters: '(', ')' and '*', return true if s is valid.

    The following rules define a valid string:

    - Any left parenthesis '(' must have a corresponding right parenthesis ')'.
    - Any right parenthesis ')' must have a corresponding left parenthesis '('.
    - Left parenthesis '(' must go before the corresponding right parenthesis ')'.
    - '*' could be treated as a single right parenthesis ')' or a single left parenthesis '(' or an empty string "".

    Example 1:

    Input: s = "()"
    Output: true

    Example 2:

    Input: s = "(*)"
    Output: true

    Example 3:

    Input: s = "(*))"
    Output: true

    Example 4:

    Input: s = "("
    Output: false

    Constraints:

    - 1 <= s.length <= 100
    - s[i] is '(', ')' or '*'.
*/
/*
    Best solution: greedy range of possible open counts [lo, hi]

    Complexity:
        Time:  O(n) — one scan.
        Space: O(1) — two counters.

    Approach (detail):
        1. Every choice for the '*' characters gives a balance (open - close) at each position; the
           set of balances reachable without ever going negative is always a contiguous range.
        2. Track it as [lo, hi]: '(' shifts both up, ')' shifts both down, '*' widens the range
           (lo - 1 for ')', hi + 1 for '(').
        3. If hi < 0, even turning every '*' into '(' cannot cover the ')' so far: return false.
        4. Clamp lo = max(lo, 0): negative balances are invalid choices and are dropped.
        5. At the end the string is valid iff 0 is reachable, i.e. lo == 0.
*/
/*
    Your solution: greedy [lo, hi] range of reachable balances

    Complexity:
        Time:  O(n) — one pass over s.
        Space: O(1) — lo and hi.

    Approach:
        1. '(' does ++lo, ++hi; ')' does lo = max(lo - 1, 0), --hi; '*' does lo = max(lo - 1, 0), ++hi.
        2. After each character, hi < 0 returns false.
        3. The result is lo == 0.

    Verdict: MATCHES BEST — the O(n) / O(1) range greedy. Verified exhaustively against a
        set-of-balances reference on every string of '(', ')', '*' up to length 12.
*/
class Solution {
    public:
        bool checkValidString(string s) {
            int n = s.size();
            int lo = 0, hi = 0; // balance tại 1 thời điểm luôn thuộc [lo, hi], nếu hi < 0 => false, nếu hiện tại balance = k, s[i] = '*' => balance tiếp theo có thể là: [k-1, k, k+1], [max(lo-1, 0), hi+1]
            for (char c : s){
                if      (c == '(') {++lo; ++hi;}
                else if (c == ')') {lo = max(lo-1, 0); --hi;}
                else               {lo = max(lo-1, 0); ++hi;}

                if (hi < 0) return false;
            }
            return (lo == 0);
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

    string s = "(*)(())";
    s = "(*)()";
    s = "(((((()*)(*)*))())())(()())())))((**)))))(()())()";
    cout << sol.checkValidString(s) << "\n";

    return 0;
}