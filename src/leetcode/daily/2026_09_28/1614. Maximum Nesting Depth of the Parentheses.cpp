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
    LC 1614 - Maximum Nesting Depth of the Parentheses
    https://leetcode.com/problems/maximum-nesting-depth-of-the-parentheses/

    Problem:
        Given a valid parentheses string s, return the nesting depth of s. The nesting depth is the
        maximum number of nested parentheses.

        Constraints:
            1 <= s.length <= 100
            s consists of digits 0-9 and characters '+', '-', '*', '/', '(', and ')'.
            It is guaranteed that parentheses expression s is a VPS.

    Examples:
        Input: s = "(1+(2*3)+((8)/4))+1"
        Output: 3
        Explanation: digit 8 is inside of 3 nested parentheses.

        Input: s = "(1)+((2))+(((3)))"
        Output: 3

        Input: s = "()(())((()()))"
        Output: 3

    Best solution: single scan with an open-bracket counter, tracking its maximum.

    Complexity:
        Time:  O(n) — one pass.
        Space: O(1) — two integers.

    Approach (detail):
        1. depth = 0, best = 0.
        2. On '(' : depth++, best = max(best, depth). On ')' : depth--. Other characters are ignored.
        3. depth at any position is exactly how many parentheses enclose it (s is valid, so it never
           goes negative), so the maximum over the scan is the nesting depth. No stack is needed.

    Your solution:
        MATCHES BEST — the O(n)/O(1) counter scan.
*/

class Solution {
    public:
        int maxDepth(string s) {
            int n = s.size();
            int open_brackets = 0;
            int res = 0;
            for (int i = 0; i < n; ++i){
                if (s[i] == '('){
                    ++open_brackets;
                    res = max(res, open_brackets);
                }
                else if (s[i] == ')'){
                    --open_brackets;
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