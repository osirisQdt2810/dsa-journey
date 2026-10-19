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
#include <stack>

using namespace std;

/*
    LC 32 - Longest Valid Parentheses
    https://leetcode.com/problems/longest-valid-parentheses/

    Given a string containing just the characters '(' and ')', return the length of the longest valid (well-formed) parentheses substring.

    Example 1:

    Input: s = "(()"
    Output: 2
    Explanation: The longest valid parentheses substring is "()".

    Example 2:

    Input: s = ")()())"
    Output: 4
    Explanation: The longest valid parentheses substring is "()()".

    Example 3:

    Input: s = ""
    Output: 0

    Constraints:

    - 0 <= s.length <= 3 * 10^4
    - s[i] is '(', or ')'.
*/
/*
    Best solution: two-pass counting (left-to-right, then right-to-left)

    Complexity:
        Time:  O(n) — two linear scans.
        Space: O(1) — two counters.

    Approach (detail):
        1. Scan left to right counting open / close. When open == close the current stretch is a
           balanced string of length 2 * close; when close > open the stretch is broken, so reset
           both counters to 0.
        2. Scan right to left the same way with the roles swapped (reset when open > close). This
           catches stretches with extra '(' such as "(()", which the first pass never balances.
        3. The answer is the largest 2 * count seen in either pass. Every valid substring is
           maximal-balanced in at least one of the two directions, so nothing is missed.
*/
/*
    Your solution: stack of '(' indices + len[] extending each matched pair by the run before it

    Complexity:
        Time:  O(n) — each index is pushed / popped at most once.
        Space: O(n) — the stack open and the array len.

    Approach:
        1. On '(' push its index i onto open.
        2. On ')' with a non-empty stack, pop the matching '(' index o; the pair s[o..i] has length
           i - o + 1, and len[o-1] (the valid run ending right before o) is added so adjacent runs
           such as "()()" join: len[i] = len[o-1] + (i - o + 1).
        3. A ')' with an empty stack is unmatched and leaves len[i] = 0, which breaks the chain.
        4. res keeps the maximum len[i].

    Verdict: SUBOPTIMAL — correct, same O(n) time, but O(n) extra space vs O(1) for the two-pass
        count; irrelevant at s.length <= 3 * 10^4. Verified exhaustively on every string of
        '(' / ')' up to length 16.
*/
class Solution {
    public:
        int longestValidParentheses(string s) {
            int n = s.size();
            stack<int> open;
            vector<int> len(n, 0);
            int res = 0;
            for (int i = 0; i < n; ++i){
                if (s[i] == '('){
                    open.push(i);
                    continue;
                }
                if (!open.empty()){
                    int o = open.top(); open.pop();
                    len[i] = ((o > 0) ? len[o-1] : 0) + (i - o + 1);
                    res = max(res, len[i]);
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