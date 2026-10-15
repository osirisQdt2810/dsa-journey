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
    LC 1807 - Evaluate the Bracket Pairs of a String
    https://leetcode.com/problems/evaluate-the-bracket-pairs-of-a-string/

    Problem:
        You are given a string s that contains some bracket pairs, with each pair containing a
        non-empty key (e.g. "(name)is(age)yearsold" has the keys "name" and "age"). knowledge[i] =
        [key_i, value_i] gives the value of key_i. Evaluate every bracket pair: replace the key and
        its brackets with value_i, or with "?" if the key's value is unknown. Each key appears at
        most once in knowledge; there are no nested brackets in s. Return the resulting string.

        Constraints:
            1 <= s.length <= 10^5
            0 <= knowledge.length <= 10^5
            knowledge[i].length == 2
            1 <= key_i.length, value_i.length <= 10
            s consists of lowercase English letters and round brackets '(' and ')'.
            Every open bracket '(' in s will have a corresponding close bracket ')'.
            The key in each bracket pair of s will be non-empty.
            There will not be any nested bracket pairs in s.
            key_i and value_i consist of lowercase English letters.
            Each key_i in knowledge is unique.

    Examples:
        Input: s = "(name)is(age)yearsold", knowledge = [["name","bob"],["age","two"]]
        Output: "bobistwoyearsold"

        Input: s = "hi(name)", knowledge = [["a","b"]]
        Output: "hi?"
        Explanation: the value of "name" is unknown, so "(name)" becomes "?".

        Input: s = "(a)(a)(a)aaa", knowledge = [["a","yes"]]
        Output: "yesyesyesaaa"
        Explanation: the same key may appear many times; letters outside brackets are untouched.

    Best solution: hash map key -> value + single linear scan of s.

    Complexity:
        Time:  O(|s| + K), K = total length of knowledge — each char of s and of every key/value is
               touched O(1) times (keys are <= 10 chars, so hashing a key is O(1)).
        Space: O(K + |s|) — the hash map plus the output string.

    Approach (detail):
        1. Put every (key, value) of knowledge into a hash map.
        2. Scan s left to right with a flag "inside a bracket":
           - '('  -> start collecting a new key;
           - ')'  -> look the collected key up: append its value, or "?" if absent;
           - letter -> append to the key when inside brackets, otherwise straight to the answer.
        3. Brackets never nest, so a single "current key" buffer is enough; every bracket pair is
           resolved independently, which is why one pass is correct.

    Your solution:
        MATCHES BEST — same one-pass O(|s| + K) scan, but keys are identified by a polynomial rolling
        hash (P = 31, M = 1e9+7) instead of the string itself: the map is keyed by hash(key), and each
        bracket's key hash is taken in O(1) from prefix hashes of s. It is a single-modulus hash, so it
        is correct only up to (very unlikely) collisions between 10-char keys; a string-keyed map
        removes that risk at the same complexity.
*/

class Solution {
    private:
        using hash = long long;
        using index = size_t;

        static constexpr int P = 31;
        static constexpr int M = 1e9 + 7;

        hash hash_string(string& key){
            int n = key.size();
            hash h = 0;
            for (int i = 0; i < n; ++i){
                h = (h * P + key[i] - 'a' + 1) % M;
            }
            return h;
        }

        // hash hash_string(string& str, int start, int end){

        // }

    public:
        string evaluate(string s, vector<vector<string>>& knowledge) {
            unordered_map<hash, index> klmap;
            for (size_t i = 0; i < knowledge.size(); ++i){
                auto& key = knowledge[i][0];
                klmap[hash_string(key)] = i;
            }

            int open = -1;
            int n = s.size();
            string target = "";

            vector<hash> input_hash(n, 0);
            vector<hash> pow(n, 1);
            // build hash of s
            for (int i = 0; i < n; ++i){
                input_hash[i] = (s[i] - 'a' + 1) % M;
                if (i > 0){
                    input_hash[i] += (input_hash[i-1] * P) % M;
                    pow[i] = (pow[i-1] * P) % M;
                }
            }

            for (int i = 0; i < n; ++i){
                if (s[i] == '('){
                    open = i;
                }
                else if (s[i] == ')'){
                    // hash(s[i...j]) = hash(s[j]) - P^len * hash(s[i-1]), len = j-i+1
                    // hash h = hash_string(s, open + 1, i - 1); hash(s[open+1...i-1])
                    int len = i - open - 1;
                    hash h = (input_hash[i-1] - (pow[len] * input_hash[open]) % M + M) % M;

                    if (klmap.count(h)){
                        target += knowledge[klmap[h]][1];
                    }
                    else {
                        target += '?';
                    }
                    open = -1;
                }
                else {
                    if (open < 0) target += s[i];
                }
            }
            return target;
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