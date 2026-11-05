#include <vector>
#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include <climits>

using namespace std;

/*
    LC 3291 - Minimum Number of Valid Strings to Form Target I
    https://leetcode.com/problems/minimum-number-of-valid-strings-to-form-target-i/

    You are given an array of strings words and a string target.

    A string x is called valid if x is a prefix of any string in words.

    Return the minimum number of valid strings that can be concatenated to form target. If it is not possible to form target, return -1.

    Example 1:

    Input: words = ["abc","aaaaa","bcdef"], target = "aabcdabc"
    Output: 3
    Explanation:
    The target string can be formed by concatenating:
    - Prefix of length 2 of words[1], i.e. "aa".
    - Prefix of length 3 of words[2], i.e. "bcd".
    - Prefix of length 3 of words[0], i.e. "abc".

    Example 2:

    Input: words = ["abababab","ab"], target = "ababaababa"
    Output: 2
    Explanation:
    The target string can be formed by concatenating:
    - Prefix of length 5 of words[0], i.e. "ababa".
    - Prefix of length 5 of words[0], i.e. "ababa".

    Example 3:

    Input: words = ["abcdef"], target = "xyz"
    Output: -1

    Constraints:

    - 1 <= words.length <= 100
    - 1 <= words[i].length <= 5 * 10^3
    - The input is generated such that sum(words[i].length) <= 10^5.
    - words[i] consists only of lowercase English letters.
    - 1 <= target.length <= 5 * 10^3
    - target consists only of lowercase English letters.
*/
/*
    Best solution: Aho-Corasick over words + pull DP using the longest match ending at each j

    Complexity:
        Time:  O(26 * sum|words| + |target|) — build the automaton once, then one pass over target.
        Space: O(26 * sum|words| + |target|) — goto table, fail links, depth[] and dp[].

    Approach (detail):
        1. dp[j] = min pieces forming target[0..j). dp is non-decreasing: drop the last char of the
           last piece of an optimal split of target[0..j) and you get a split of target[0..j-1)
           with <= the same number of pieces (a prefix of a prefix is still a prefix).
        2. Build Aho-Corasick on all words. After reading target[j], the state's depth d_j is the
           longest suffix of target[0..j] that is a prefix of some word.
        3. Every valid last piece ending at j starts at s >= j+1-d_j, and dp is non-decreasing,
           so the earliest start is optimal: dp[j+1] = dp[j+1-d_j] + 1 (INF when d_j == 0).
        4. Answer dp[n], or -1 if INF. (Same O(n) DP as Jump Game II once len_i is known.)
*/
/*
    Your solution: z_segtree_sol — Z-function match lengths + range-chmin / point-query segment tree

    Complexity:
        Time:  O(sum|words| + W * |target| + |target| log |target|) — one Z pass per word over
               word|target (W = words.length), then T point queries + T range updates.
        Space: O(max|word| + |target|) — the Z array, len[] and the 4T tree.

    Approach:
        1. build_z_function: for each word run Z on word + '|' + target; len[i] = max over words
           of z[wlen+1+i] = longest prefix of some word matching target starting at i.
        2. range_update_point_query_tree keeps one tag it[t] per node (no lazy push):
           update_range writes min(it[t], val) on the O(log T) covering nodes and stops;
           query_point(k) returns the min of the tags on the root-to-leaf path of k. Correct
           because min is commutative/associative and every update covering k leaves its tag on
           exactly one node of that path.
        3. Position t of the tree holds dp[t] (prefix length t). For t = 1..T:
           prev = dp[t-1] (0 when t == 1, else query_point(t-1)); if finite,
           update_range(t, t+len[t-1]-1, prev+1) — pieces target[t-1 .. t-1+L) for L <= len[t-1].
        4. Return query_point(T), or -1 if INF.

    Also in the file (not called): trie_sol — trie of words + dp walking the trie from every start,
        O(sum|words| + T * min(T, max|word|)), correct.

    Verdict: SUBOPTIMAL — O(sum + W*T + T log T) vs O(26*sum + T) for Aho-Corasick; at W <= 100,
        T <= 5*10^3 that is ~6*10^5 steps, so it passes easily. Verified against a brute-force DP
        on 20000 random cases (+ a 5000-char all-'a' stress case).
*/
class Solution {
    public:
        int minValidStrings(vector<string>& words, string target) {
            int T = target.size();
            int N = words.size();

            auto trie_sol = [&]() -> int {
                struct node {
                    node* child[26];
                    int prefix{0};
                };

                node* root = new node();

                auto insert = [&](string& word) -> void {
                    node* curr = root;
                    for (auto& c : word){
                        int i = c - 'a';
                        if (!curr->child[i]){
                            curr->child[i] = new node();
                        }
                        curr = curr->child[i];
                        ++curr->prefix;
                    }
                };

                for (auto& word : words){
                    insert(word);
                }

                vector<int> dp(T+1, INT_MAX);
                dp[0] = 0;

                for (int t = 1; t <= T; ++t){
                    node* curr = root;

                    if (dp[t-1] == INT_MAX) continue;

                    for (int k = t; k <= T; ++k){
                        int c = target[k-1]-'a';
                        if (!curr->child[c]) break;

                        dp[k] = min(dp[k], dp[t-1] + 1);
                        curr = curr->child[c];
                    }
                }
                return (dp[T] == INT_MAX) ? -1 : dp[T];
            };

            auto z_segtree_sol = [&]() -> int {
                struct range_update_point_query_tree {
                    int N;
                    vector<int> it;

                    range_update_point_query_tree(int n) : N(n), it(4*n+1, INT_MAX){}

                    void update_range(int t, int l, int r, int X, int Y, int val){
                        // case 1: [l...r][X...Y] or [X...Y][l...r]
                        if (r < X || Y < l) return;
                        // case 2: [X...l...r...Y]
                        if (X <= l && r <= Y){
                            it[t] = min(it[t], val);
                            return;
                        }
                        int m = (l+r)/2;
                        update_range(2*t,l,m,X,Y,val);
                        update_range(2*t+1,m+1,r,X,Y,val);
                        // not updated parent
                    }

                    void update_range(int L, int R, int val){
                        update_range(1,1,N,L,R,val);
                    }

                    int query_point(int t, int l, int r, int k){
                        // case 1: [l...r][k...k] or [k...k][l...r]
                        if (r < k || k < l) return INT_MAX;
                        if (l == r) return it[t];
                        int m = (l+r)/2;
                        return min(it[t], (k<=m) ? query_point(2*t,l,m,k) : query_point(2*t+1,m+1,r,k));
                    }

                    int query_point(int k){
                        return query_point(1,1,N,k);
                    }
                };

                auto build_z_function = [&](vector<string>& words, string& target) -> vector<int> {
                    auto z_func = [&](string& word, string& target, vector<int>& len){
                        int wlen = word.size();
                        int tlen = target.size();
                        word = word + '|' + target;
                        int sz = word.size();

                        vector<int> z(sz, 0);

                        int l = 0, r = 0;
                        for (int i = 1; i < sz; ++i){
                            if (r > i) z[i] = min(r-i, z[i-l]);
                            while (i+z[i] < sz && word[z[i]+i] == word[z[i]]) z[i]++;
                            if (i+z[i] > r){
                                l = i;
                                r = i+z[i];
                            }
                        }

                        for (int i = 0; i < tlen; ++i){
                            len[i] = max(len[i], z[wlen+1 + i]);
                        }
                    };

                    vector<int> len(T, 0);
                    for (auto& word : words){
                        z_func(word, target, len);
                    }
                    return len;
                };

                range_update_point_query_tree tree(T);
                auto len = build_z_function(words, target);

                for (int t = 0; t < T; ++t){
                    cout << len[t] << " ";
                }
                cout << "\n";

                for (int t = 1; t <= T; ++t){
                    int prev = (t == 1) ? 0 : tree.query_point(t-1);
                    if (prev == INT_MAX) continue;
                    tree.update_range(t, t + len[t-1]-1, prev+1);
                }
                int res = tree.query_point(T);
                return (res == INT_MAX) ? -1 : res;
            };

            return z_segtree_sol();
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
    auto check = [&](int i, int got, int expected) {
        ++total;
        if (got == expected) {
            ++passed;
            cout << "Example " << i << ": PASS\n";
        } else {
            cout << "Example " << i << ": FAIL  got=" << got << "  expected=" << expected << "\n";
        }
    };
    {
        vector<string> words = {"abc", "aaaaa", "bcdef"};
        string target = "aabcdabc";
        check(1, Solution().minValidStrings(words, target), 3);
    }
    {
        vector<string> words = {"abababab", "ab"};
        string target = "ababaababa";
        check(2, Solution().minValidStrings(words, target), 2);
    }
    {
        vector<string> words = {"abcdef"};
        string target = "xyz";
        check(3, Solution().minValidStrings(words, target), -1);
    }
    cout << passed << "/" << total << " passed\n";
    return 0;
    // test-lc:end
}