#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <deque>

using namespace std;

/*
    LC 2193 - Minimum Number of Moves to Make Palindrome
    https://leetcode.com/problems/minimum-number-of-moves-to-make-palindrome/

    You are given a string s consisting only of lowercase English letters.

    In one move, you can select any two adjacent characters of s and swap them.

    Return the minimum number of moves needed to make s a palindrome.

    Note that the input will be generated such that s can always be converted to a palindrome.

    Example 1:

    Input: s = "aabb"
    Output: 2
    Explanation:
    We can obtain two palindromes from s, "abba" and "baab".
    - We can obtain "abba" from s in 2 moves: "aabb" -> "abab" -> "abba".
    - We can obtain "baab" from s in 2 moves: "aabb" -> "abab" -> "baab".
    Thus, the minimum number of moves needed to make s a palindrome is 2.

    Example 2:

    Input: s = "letelt"
    Output: 2
    Explanation:
    One of the palindromes we can obtain from s in 2 moves is "lettel".
    One of the ways we can obtain it is "letelt" -> "letetl" -> "lettel".
    Other palindromes such as "tleelt" can also be obtained in 2 moves.
    It can be shown that it is not possible to obtain a palindrome in less than 2 moves.

    Constraints:

    - 1 <= s.length <= 2000
    - s consists only of lowercase English letters.
    - s can be converted to a palindrome using a finite number of moves.
*/
/*
    Best solution: greedy outer-in pairing + inversion count (Fenwick tree)

    Complexity:
        Time:  O(n log n) — one O(n) pass assigns targets, then n Fenwick query/update pairs.
        Space: O(n) — per-letter position deques, target[] and the Fenwick array.

    Approach (detail):
        1. Greedy: the leftmost remaining character keeps the left end; its partner is the
           RIGHTMOST remaining copy of the same letter (cheapest to drag to the right end).
           An exchange argument shows this never loses. The single odd-count character goes
           to the middle.
        2. Turn that into final positions: scan i left to right; for each unassigned i take
           j = last unassigned index with s[j] == s[i]; give target[i] = k, target[j] = n-1-k
           and increment k (k = number of pairs placed). If j == i it is the odd character:
           target[i] = n/2.
        3. Adjacent swaps needed to realise a permutation = its number of inversions (each swap
           fixes exactly one out-of-order pair). Count them with a Fenwick tree over target
           values: for each i, add (#earlier targets) - (#earlier targets <= target[i]).
        4. n <= 2000, so the direct O(n^2) simulation of the same greedy also passes.
*/
/*
    Your solution: inversion_sol — greedy target positions + Fenwick inversion count

    Complexity:
        Time:  O(n log n) — target assignment is O(n); each Fenwick query/inc is O(log n).
        Space: O(n) — pos[26] deques, target[] and the fenwick bits.

    Approach:
        1. pos[c] holds the indices of letter c in increasing order.
        2. Scan i; skip if target[i] is set. Take p = pos[s[i]] (p.front() == i): if p.size() == 1
           it is the odd letter -> target[i] = n/2; otherwise j = p.back(), target[i] = k,
           target[j] = n-1-k, ++k, pop both ends.
        3. fenwick tree(n): for each i, smaller = tree.query_sum(target[i]+1),
           inversions += i - smaller, tree.inc(target[i]+1). Return inversions.

    Also in the file (not called): greedy — two-sided O(n^2) simulation that shifts the string,
        correct; wrong_deque_sol — reads s[i]/s[n-1-i] of already-paired cells and uses raw index
        distance, incorrect (e.g. "bbbba"); deque_sol — two-sided greedy on original indices with
        per-letter deques and a Fenwick count of dead cells, O(n log n), correct.

    Verdict: MATCHES BEST — the optimal greedy + inversion-count algorithm.
        Verified against BFS over all adjacent-swap sequences on 4000 random strings (len <= 9).
*/
class Solution {
    public:
        int minMovesToMakePalindrome(string s) {
            int n = s.size();

            auto greedy = [&]() -> int {
                int min_moves = 0;
                for (int i = 0; i < n/2; ++i){
                    if (s[i] == s[n-1-i]) continue;
                    // a X .... Y b
                    // find a from b->Y...
                    int a_idx = n-2-i;
                    int b_idx = i+1;
                    while (a_idx > i && s[a_idx] != s[i]) --a_idx;
                    while (b_idx < n-i-1 && s[b_idx] != s[n-i-1]) ++b_idx;
                    int right_move = n-i-1-a_idx;
                    int left_move = b_idx-i;
                    // swap
                    if (right_move < left_move){
                        for (int j = a_idx; j < n-i-1; ++j){
                            s[j] = s[j+1];
                        }
                        s[n-i-1] = s[i];
                        min_moves += right_move;
                    }
                    else {
                        for (int j = b_idx; j > i; --j){
                            s[j] = s[j-1];
                        }
                        s[i] = s[n-i-1];
                        min_moves += left_move;
                    }
                }

                return min_moves;
            };

            auto wrong_deque_sol = [&]() -> int {
                deque<int> chars[26];
                int min_moves = 0;
                for (int i = 0; i < n; ++i){
                    chars[s[i]-'a'].push_back(i);
                }

                for (int i = 0; i < n/2; ++i){
                    auto& head = chars[s[i]-'a'];
                    auto& tail = chars[s[n-i-1]-'a'];
                    int l1 = i, r1 = head.back();
                    int l2 = tail.front(), r2 = n-i-1;
                    int right_move = n-i-1-r1;
                    int left_move = l2-i;
                    if (right_move < left_move){
                        head.pop_front();
                        head.pop_back();
                        min_moves += right_move;
                    }
                    else {
                        tail.pop_front();
                        tail.pop_back();
                        min_moves += left_move;
                    }
                }
                return min_moves;
            };

            auto deque_sol = [&]() -> int {
                deque<int> chars[26];
                int min_moves = 0;
                for (int i = 0; i < n; ++i){
                    chars[s[i]-'a'].push_back(i);
                }

                struct fenwick {
                    int N;
                    vector<int> bits;

                    fenwick(int n) : N(n+1), bits(N, 0){}

                    int query(int k){
                        int sum = 0;
                        while (k > 0){
                            sum += bits[k];
                            k -= k & (-k);
                        }
                        return sum;
                    }

                    void update(int k){
                        while (k < N){
                            ++bits[k];
                            k += k & (-k);
                        }
                    }

                    int query_dead(int l, int r){
                        return query(r+1) - query(l);
                    }

                    void update_dead(int k){
                        update(k+1);
                    }
                };

                fenwick tree(n);

                int L = 0, R = n-1;
                while (true){
                    while (L < R && tree.query_dead(L, L)) ++L;
                    while (L < R && tree.query_dead(R, R)) --R;
                    if (L >= R) break;
                    auto& head = chars[s[L]-'a'];
                    auto& tail = chars[s[R]-'a'];
                    int l1 = L, r1 = head.back();
                    int l2 = tail.front(), r2 = R;
                    // int move1 = r2 - r1 - dead(r1, r2-1);  // which mean: alive(r1,r2-1)
                    int move1 = r2-r1 - tree.query_dead(r1,r2-1);
                    // int move2 = l2 - l1 - dead(l1+1, l2);   // which mean: alive(l1+1,l2)
                    int move2 = l2-l1 - tree.query_dead(l1+1,l2);
                    if (move1 < move2){
                        head.pop_front();
                        head.pop_back();
                        // update_dead(l1,r1);
                        tree.update_dead(l1);
                        tree.update_dead(r1);
                        min_moves += move1;
                    }
                    else {
                        tail.pop_front();
                        tail.pop_back();
                        // update_dead(l2,r2);
                        tree.update_dead(l2);
                        tree.update_dead(r2);
                        min_moves += move2;
                    }
                }
                return min_moves;
            };

            auto inversion_sol = [&]() -> int {
                // 1. gán vị trí đích: duyệt trái -> phải, ký tự i chưa gán ghép với bản sao xa nhất bên phải
                //    cặp thứ k: i -> k, j -> n-1-k; ký tự lẻ -> n/2
                // 2. số swap kề nhau để đưa mọi ký tự về đích = số nghịch thế của target
                deque<int> pos[26];
                for (int i = 0; i < n; ++i) pos[s[i]-'a'].push_back(i);

                vector<int> target(n, -1);
                int k = 0;
                for (int i = 0; i < n; ++i){
                    if (target[i] != -1) continue;
                    auto& p = pos[s[i]-'a'];          // p.front() == i
                    if (p.size() == 1){
                        target[i] = n/2;
                        p.pop_front();
                    }
                    else {
                        int j = p.back();
                        target[i] = k;
                        target[j] = n-1-k;
                        ++k;
                        p.pop_front();
                        p.pop_back();
                    }
                }

                struct fenwick {
                    int N;
                    vector<int> bits;

                    fenwick(int n) : N(n+1), bits(N, 0){}

                    int query_sum(int k){
                        int sum = 0;
                        while (k > 0){
                            sum += bits[k];
                            k -= k & (-k);
                        }
                        return sum;
                    }

                    void inc(int k){
                        while (k < N){
                            ++bits[k];
                            k += k & (-k);
                        }
                    }
                };

                fenwick tree(n);
                int inversions = 0;
                for (int i = 0; i < n; ++i){
                    int smaller = tree.query_sum(target[i]+1);    // số target đã thấy <= target[i]
                    inversions += i - smaller;
                    tree.inc(target[i]+1);
                }
                return inversions;
            };

            return inversion_sol();
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