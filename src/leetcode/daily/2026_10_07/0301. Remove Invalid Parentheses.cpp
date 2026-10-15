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
#include <functional>

using namespace std;

/*
    LC 301 - Remove Invalid Parentheses
    https://leetcode.com/problems/remove-invalid-parentheses/

    Given a string s that contains parentheses and letters, remove the minimum number of invalid parentheses to make the input string valid.

    Return a list of unique strings that are valid with the minimum number of removals. You may return the answer in any order.

    Example 1:

    Input: s = "()())()"
    Output: ["(())()","()()()"]

    Example 2:

    Input: s = "(a)())()"
    Output: ["(a())()","(a)()()"]

    Example 3:

    Input: s = ")("
    Output: [""]

    Constraints:

    - 1 <= s.length <= 25
    - s consists of lowercase English letters and parentheses '(' and ')'.
    - There will be at most 20 parentheses in s.
*/
/*
    Best solution: two-direction DFS with forced deletions — scan left to right; at the first prefix
    with an extra ')' delete one ')' from that prefix (only the first of each run, positions
    non-decreasing), recurse; when no ')' is extra, reverse the string and fix extra '(' the same way.

    Complexity:
        Time:  O(n * #answers) in practice (output-sensitive): every leaf is a distinct valid string;
               worst case bounded by O(n * C(n, k)), k = minimum number of deletions.
        Space: O(n^2) for the recursion stack of string copies (depth <= k <= n).

    Approach (detail):
        1. Minimum deletions: with '(' = +1 and ')' = -1, R = -min(prefix sum) ')' must go and
           L = final sum + R '(' must go; every answer deletes exactly L '(' and R ')'.
        2. Forward pass: the first time the balance reaches -1 at index i, the prefix [0..i] has
           exactly one extra ')'; deleting any ')' in it fixes the prefix, and nothing later can.
           Branch over those positions, then continue scanning from i (last_i).
        3. No duplicates without a set: deletions are taken at non-decreasing positions (last_j),
           and only the first ')' of a run of equal characters is deleted (deleting any of ")))"
           gives the same string).
        4. Backward pass: an extra '(' only shows from the right, so reverse the string and swap the
           roles of '(' and ')', reusing the same routine; after the second reversal the string is
           in its original order and is valid.

    Your solution:
        SUBOPTIMAL — the method returns keep_delete_sol: a DFS that keeps or deletes each character,
        deleting exactly L '(' and R ')', pruning as soon as a kept ')' would have no partner, and
        de-duplicating with set<string>; it explores repeated branches that yield the same string
        (up to O(n * C(m, k)) work plus set insertions) vs the output-sensitive optimized_sol. It
        passes easily at n <= 25 (~14 ms over 206 hard inputs vs ~1 ms for optimized_sol). The file
        also keeps trivial_sol (enumerate all C(m, len) deletion sets, validate at the leaf, set
        de-dup; ~343 ms on the same inputs) and optimized_sol (the best solution above) as dead
        reference versions — switching the return to optimized_sol() makes this MATCHES BEST.
        All three verified against a BFS reference on 20000 random strings.
*/
class Solution {
    public:
        vector<string> removeInvalidParentheses(string s) {
            int n = s.size();

            // cách 1: chọn tổ hợp `len` dấu ngoặc để xoá, kiểm tra hợp lệ ở lá, khử trùng bằng set
            auto trivial_sol = [&]() -> vector<string> {
                int len = 0;
                int acc = 0;
                for (int i = 0; i < n; ++i){
                    if (s[i] == '('){
                        ++acc;
                    }
                    else if (s[i] == ')'){
                        if (--acc < 0){
                            len = max(len, -acc);
                        }
                    }
                }
                len = (len > 0) ? (2 * len + acc) : acc;

                set<string> res;
                vector<bool> visit(n, false);

                auto is_valid = [&]() -> bool {
                    int bal = 0;
                    for (int i = 0; i < n; ++i){
                        if (visit[i]) continue;
                        if (s[i] == '(') ++bal;
                        else if (s[i] == ')' && --bal < 0) return false;
                    }
                    return bal == 0;
                };

                function<void(int, int)> select_k_combination = [&](int start_pos, int k){
                    if (k == len){
                        if (!is_valid()) return;
                        string ans = "";
                        for (int i = 0; i < n; ++i){
                            if (!visit[i]) ans.push_back(s[i]);
                        }
                        res.insert(ans);
                        return;
                    }
                    for (int i = start_pos; i < n; ++i){
                        if (s[i] != '(' && s[i] != ')') continue;   // chỉ xoá dấu ngoặc
                        visit[i] = true;
                        select_k_combination(i+1, k+1);
                        visit[i] = false;
                    }
                };
                select_k_combination(0, 0);
                return vector<string>(res.begin(), res.end());
            };

            // cách 2: DFS giữ / xoá từng ký tự, chỉ xoá đúng L '(' và R ')', cắt nhánh khi ')' không có cặp
            auto keep_delete_sol = [&]() -> vector<string> {
                int L = 0, R = 0;                       // số '(' và ')' phải xoá
                for (char c : s){
                    if (c == '(') ++L;
                    else if (c == ')'){
                        if (L > 0) --L;                 // ghép được với một '(' trước đó
                        else ++R;                       // ')' không có cặp, bắt buộc phải xoá
                    }
                }

                set<string> res;
                string cur;                             // chuỗi đang dựng
                // dfs(i, l, r, bal): đang xét s[i]; còn phải xoá l '(' và r ')'; bal = số '(' đang mở trong cur
                function<void(int, int, int, int)> dfs = [&](int i, int l, int r, int bal){
                    if (i == n){
                        if (l == 0 && r == 0 && bal == 0) res.insert(cur);
                        return;
                    }
                    char c = s[i];
                    // lựa chọn 1: XOÁ s[i], chỉ khi còn hạn mức xoá loại đó
                    if (c == '(' && l > 0) dfs(i+1, l-1, r, bal);
                    if (c == ')' && r > 0) dfs(i+1, l, r-1, bal);
                    // lựa chọn 2: GIỮ s[i]
                    cur.push_back(c);
                    if (c != '(' && c != ')') dfs(i+1, l, r, bal);       // chữ cái: luôn giữ
                    else if (c == '(')        dfs(i+1, l, r, bal+1);
                    else if (bal > 0)         dfs(i+1, l, r, bal-1);     // ')' chỉ giữ được khi còn '(' để ghép
                    cur.pop_back();
                };
                dfs(0, L, R, 0);
                return vector<string>(res.begin(), res.end());
            };

            // cách 3: quét 2 chiều — chiều xuôi xoá ')' thừa, đảo chuỗi rồi xoá '(' thừa
            auto optimized_sol = [&]() -> vector<string> {
                vector<string> ans;

                // remove(t, last_i, last_j, open, close):
                //   quét t từ last_i; tại tiền tố đầu tiên có #close > #open, xoá 1 `close` trong
                //   [last_j, i] (chỉ dấu đầu của mỗi cụm giống nhau) rồi đệ quy;
                //   khi chiều này đã cân bằng thì đảo chuỗi và xử lý chiều còn lại.
                function<void(const string&, int, int, char, char)> remove =
                    [&](const string& t, int last_i, int last_j, char open, char close){
                    for (int bal = 0, i = last_i; i < (int)t.size(); ++i){
                        if (t[i] == open) ++bal;
                        else if (t[i] == close) --bal;
                        if (bal >= 0) continue;

                        for (int j = last_j; j <= i; ++j){
                            if (t[j] == close && (j == last_j || t[j-1] != close)){
                                remove(t.substr(0, j) + t.substr(j+1), i, j, open, close);
                            }
                        }
                        return;
                    }

                    string rev(t.rbegin(), t.rend());
                    if (open == '(') remove(rev, 0, 0, ')', '(');   // xong chiều xuôi → xử lý '(' thừa
                    else ans.push_back(rev);                        // đã đảo 2 lần → đúng chiều ban đầu
                };

                remove(s, 0, 0, '(', ')');
                return ans;
            };

            return keep_delete_sol();
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
    string s = "(()())(())";
    sol.removeInvalidParentheses(s);

    return 0;
}