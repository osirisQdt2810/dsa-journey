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
    LC 921 - Minimum Add to Make Parentheses Valid
    https://leetcode.com/problems/minimum-add-to-make-parentheses-valid/

    A parentheses string is valid if and only if:

    - It is the empty string,
    - It can be written as AB (A concatenated with B), where A and B are valid strings, or
    - It can be written as (A), where A is a valid string.

    You are given a parentheses string s. In one move, you can insert a parenthesis at any position of the string.

    - For example, if s = "()))", you can insert an opening parenthesis to be "(()))" or a closing parenthesis to be "())))".

    Return the minimum number of moves required to make s valid.

    Example 1:

    Input: s = "())"
    Output: 1

    Example 2:

    Input: s = "((("
    Output: 3

    Constraints:

    - 1 <= s.length <= 1000
    - s[i] is either '(' or ')'.
*/
/*
    Best solution: one-pass greedy balance counter.

    Complexity:
        Time:  O(n) — single scan.
        Space: O(1) — two counters.

    Approach (detail):
        1. open = number of '(' still waiting for a ')'; add = insertions made so far.
        2. On '(' : open++. On ')' : if open > 0 it closes one (open--), otherwise this ')' has no
           partner and a '(' must be inserted before it (add++).
        3. At the end every still-open '(' needs a ')' inserted: answer = add + open.
        Each insertion is forced at the moment it is counted, so the total is minimal.

    Your solution:
        MATCHES BEST — same O(n)/O(1) greedy written with a running balance acc: a negative acc is
        the number of unmatched ')' so far, paid off (ans += -acc) when the next '(' arrives, and
        |acc| at the end covers whichever side is left over. Verified exhaustively for all strings
        up to length 14.
*/
class Solution {
    public:
        int minAddToMakeValid(string s) {
            int ans = 0;
            int n = s.size();
            int acc = 0;
            for (int i = 0; i < n; ++i){
                if (s[i] == '('){
                    if (acc < 0){
                        ans += (-acc);
                        acc = 0;
                    }
                    ++acc;
                }
                else {
                    --acc;
                }
            }
            return ans + abs(acc);
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