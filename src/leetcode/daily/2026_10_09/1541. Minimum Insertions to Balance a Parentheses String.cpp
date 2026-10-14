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
    LC 1541 - Minimum Insertions to Balance a Parentheses String
    https://leetcode.com/problems/minimum-insertions-to-balance-a-parentheses-string/

    Problem:
        Given a parentheses string s containing only the characters '(' and ')'. A parentheses string
        is balanced if:
        - Any left parenthesis '(' must have a corresponding two consecutive right parenthesis '))'.
        - Left parenthesis '(' must go before the corresponding two consecutive right parenthesis '))'.
        In other words, '(' is an opening parenthesis and '))' is a closing one (e.g. "())",
        "())(())))" and "(())())))" are balanced; ")()", "()))" and "(()))" are not).
        You can insert '(' and ')' at any position. Return the minimum number of insertions needed to
        make s balanced.

        Constraints:
            1 <= s.length <= 10^5
            s consists of '(' and ')' only.

    Examples:
        Input: s = "(()))"
        Output: 1
        Explanation: the first '(' only has one ')'; add one more ')' at the end -> "(())))".

        Input: s = "())"
        Output: 0

        Input: s = "))())("
        Output: 3
        Explanation: add '(' to match the first '))', and add '))' to match the last '('.

    Best solution: one-pass greedy counting the ')' still needed.

    Complexity:
        Time:  O(n) — single scan.
        Space: O(1) — two counters.

    Approach (detail):
        1. Keep need = number of ')' still required by the open '(' so far, and ans = insertions.
        2. On '(' : if need is odd, the previous '(' has a dangling single ')' that can no longer be
           completed by a ')' (it must be consecutive), so insert one ')' now (ans++, need--).
           Then need += 2.
        3. On ')' : need--. If need < 0 there was no open '(' to close, so insert a '(' (ans++)
           which demands two ')' — this one plus one more (need += 2).
        4. At the end every remaining needed ')' must be inserted: answer = ans + need.
        Greedy is optimal because each fix is forced at the moment it is applied: an odd count before
        a new '(' can never be paired consecutively later, and a ')' with no opener can only be fixed
        by inserting a '(' before it.

    Your solution:
        MATCHES BEST — the O(n)/O(1) greedy above (right_needed plays the role of need); checked
        exhaustively against a reference for every string up to length 14.
*/

class Solution {
    public:
        int minInsertions(string s) {
            int n = s.size();

            int ans = 0, right_needed = 0;
            for (int i = 0; i < n; ++i){
                if (s[i] == '('){
                    if (right_needed % 2){
                        ++ans;
                        --right_needed;
                    }
                    right_needed += 2;
                    continue;
                }
                --right_needed;
                if (right_needed < 0){
                    ++ans;
                    right_needed += 2;
                }
            }
            // cout << ans << " " << right_needed << "\n";
            return ans + right_needed;
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
    string s = "(()))(()))()())))";
    // s = "(()))";
    // s = "(()))";
    // s = "))";

    cout << sol.minInsertions(s);

    return 0;
}