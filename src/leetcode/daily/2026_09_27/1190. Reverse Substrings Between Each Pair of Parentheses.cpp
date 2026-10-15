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
    LC 1190 - Reverse Substrings Between Each Pair of Parentheses
    https://leetcode.com/problems/reverse-substrings-between-each-pair-of-parentheses/

    Problem:
        You are given a string s that consists of lower case English letters and brackets. Reverse
        the strings in each pair of matching parentheses, starting from the innermost one. The result
        should not contain any brackets.

        Constraints:
            1 <= s.length <= 2000
            s only contains lower case English characters and parentheses.
            It is guaranteed that all parentheses are balanced.

    Examples:
        Input: s = "(abcd)"
        Output: "dcba"

        Input: s = "(u(love)i)"
        Output: "iloveu"
        Explanation: "love" is reversed first, then the whole string is reversed.

        Input: s = "(ed(et(oc))el)"
        Output: "leetcode"
        Explanation: reverse "oc", then "etco", and finally the whole string.

    Best solution: "wormhole" traversal — pair up matching brackets with a stack, then walk the
    string once, jumping to the partner bracket and flipping direction at every bracket.

    Complexity:
        Time:  O(n) — one pass to pair brackets, one pass where every index is visited at most twice.
        Space: O(n) — the pair[] array, the stack and the output.

    Approach (detail):
        1. Use a stack of '(' indices; on ')' pop j and record pair[i] = j, pair[j] = i.
        2. Walk with index i and direction d = +1, starting at 0.
           - On a letter, append it to the answer.
           - On a bracket, teleport to its partner (i = pair[i]) and reverse direction (d = -d),
             then continue stepping by d.
        3. Why it works: entering a pair from one end and leaving through the other end reverses its
           content. A substring nested k levels deep is traversed in direction (-1)^k, which is exactly
           the composition of the k reversals. Each character is emitted exactly once, so the walk
           emits the final string directly, without the O(n^2) repeated reversals.

    Your solution:
        MATCHES BEST — reverseParentheses (the method LeetCode calls) is the O(n) wormhole walk
        described above. reverseParenthesesBruteforce is dead code kept for reference: it reverses
        each closed segment in place, O(n^2) time in the worst case.
*/

class Solution {
    public:
        string reverseParentheses(string s){
            // wormhole algorithm
            int n = s.size();
            stack<int> open;
            vector<int> pair(n);
            for (int i = 0; i < n; ++i){
                if (s[i] == '('){
                    open.push(i);
                }
                else if (s[i] == ')'){
                    int j = open.top(); open.pop();
                    pair[i] = j;
                    pair[j] = i;
                }
            }

            // build image
            string res;
            int d = 1;
            for (int i = 0; i < n; i += d){
                if (s[i] == '(' || s[i] == ')'){
                    i = pair[i];
                    d = -d;
                }
                else {
                    res.push_back(s[i]);
                }
            }
            return res;
        }

        string reverseParenthesesBruteforce(string s) {
            // (ed(et(oc))el)
            // (ed(etco)el)
            // (edcoteel)
            // (leetcode)
            //       0 ... i ... j ... n-1 => 0...(j...i)...n-1
            //  0 (1 (2 3)) (4 5 (6 7)) => (4 5 7 6)
            //  ((0 1))
            //  0 (3 2 1) => với k thuộc [i, j] thì k->i+j-k
            int n = s.size();
            int brackets = 0;

            stack<int> open;
            string res;
            for (int i = 0; i < n; ++i){
                int bracket = (s[i] == '(') || (s[i] == ')');
                brackets += bracket;

                if (!bracket) res.push_back(s[i]);

                if (s[i] == '('){
                    open.push(res.size());
                }
                else if (s[i] == ')'){
                    int s_idx = open.top();
                    open.pop();
                    int e_idx = res.size() - 1;
                    while (s_idx < e_idx){
                        swap(res[s_idx++], res[e_idx--]);
                    }
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
    string inp = "(ed(et(oc))el)";
    cout << sol.reverseParentheses(inp) << "\n";

    return 0;
}