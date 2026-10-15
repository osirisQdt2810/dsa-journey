#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <queue>
#include <stack>
#include <algorithm>
#include <climits>

using namespace std;

/*
    LC 856 - Score of Parentheses
    https://leetcode.com/problems/score-of-parentheses/

    Given a balanced parentheses string s, return the score of the string.

    The score of a balanced parentheses string is based on the following rule:

    - "()" has score 1.
    - AB has score A + B, where A and B are balanced parentheses strings.
    - (A) has score 2 * A, where A is a balanced parentheses string.

    Example 1:

    Input: s = "()"
    Output: 1

    Example 2:

    Input: s = "(())"
    Output: 2

    Example 3:

    Input: s = "()()"
    Output: 2

    Constraints:

    - 2 <= s.length <= 50
    - s consists of only '(' and ')'.
    - s is a balanced parentheses string.
*/
/*
    Best solution: depth counting — every innermost "()" contributes 2^depth, where depth is the
    number of '(' still open around it.

    Complexity:
        Time:  O(n) — one scan.
        Space: O(1) — just the depth counter and the sum.

    Approach (detail):
        1. Expand the rules: (A) doubles everything inside it and AB adds, so the score is a sum over
           the innermost "()" pairs, each multiplied by 2 once per enclosing pair.
        2. Scan with depth = number of open '('. On '(' : depth++. On ')' : depth--, and if the
           previous character is '(' this closes an innermost "()", so add 1 << depth.
        3. Example "(()(()))": the "()" at depth 1 adds 2, the "()" at depth 2 adds 4, total 6.

    Your solution:
        SUBOPTIMAL — a stack of '(' indices with score[] per open position: a ")" right after its
        "(" scores 1, otherwise the closed pair passes 2 * score up to the enclosing '(' (or to ans at
        the top level). Same O(n) time, but O(n) extra space for the stack and score[] vs O(1) for
        depth counting; irrelevant at n <= 50. Verified on all 6917 balanced strings up to length 18
        and on the deepest n = 50 case.
*/
class Solution {
    public:
        int scoreOfParentheses(string s) {
            int ans = 0;
            int n = s.size();
            stack<int> open;
            vector<int> score(n, 0);
            for (int i = 0; i < n; ++i){
                if (s[i] == '('){
                    open.push(i);
                    continue;
                }
                int top = open.top(); open.pop();
                if (i == top+1) score[top] = 1;
                if (open.empty()){
                    ans += score[top];
                }
                else {
                    score[open.top()] += 2 * score[top];
                }
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
    string  s = "()";
     s = "(())";
     s = "()()";
    cout << sol.scoreOfParentheses(s);

    return 0;
}