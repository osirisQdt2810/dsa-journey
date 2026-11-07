#include <algorithm>
#include <vector>
#include <iostream>
#include <set>

using namespace std;

/*
    LC 1649 - Create Sorted Array through Instructions
    https://leetcode.com/problems/create-sorted-array-through-instructions/

    Given an integer array instructions, you are asked to create a sorted array from the elements in instructions. You start with an empty container nums. For each element from left to right in instructions, insert it into nums. The cost of each insertion is the minimum of the following:

    - The number of elements currently in nums that are strictly less than instructions[i].
    - The number of elements currently in nums that are strictly greater than instructions[i].

    For example, if inserting element 3 into nums = [1,2,3,5], the cost of insertion is min(2, 1) (elements 1 and 2 are less than 3, element 5 is greater than 3) and nums will become [1,2,3,3,5].

    Return the total cost to insert all elements from instructions into nums. Since the answer may be large, return it modulo 10^9 + 7

    Example 1:

    Input: instructions = [1,5,6,2]
    Output: 1
    Explanation: Begin with nums = [].
    Insert 1 with cost min(0, 0) = 0, now nums = [1].
    Insert 5 with cost min(1, 0) = 0, now nums = [1,5].
    Insert 6 with cost min(2, 0) = 0, now nums = [1,5,6].
    Insert 2 with cost min(1, 2) = 1, now nums = [1,2,5,6].
    The total cost is 0 + 0 + 0 + 1 = 1.

    Example 2:

    Input: instructions = [1,2,3,6,5,4]
    Output: 3
    Explanation: Begin with nums = [].
    Insert 1 with cost min(0, 0) = 0, now nums = [1].
    Insert 2 with cost min(1, 0) = 0, now nums = [1,2].
    Insert 3 with cost min(2, 0) = 0, now nums = [1,2,3].
    Insert 6 with cost min(3, 0) = 0, now nums = [1,2,3,6].
    Insert 5 with cost min(3, 1) = 1, now nums = [1,2,3,5,6].
    Insert 4 with cost min(3, 2) = 2, now nums = [1,2,3,4,5,6].
    The total cost is 0 + 0 + 0 + 0 + 1 + 2 = 3.

    Example 3:

    Input: instructions = [1,3,3,3,2,4,2,1,2]
    Output: 4
    Explanation: Begin with nums = [].
    Insert 1 with cost min(0, 0) = 0, now nums = [1].
    Insert 3 with cost min(1, 0) = 0, now nums = [1,3].
    Insert 3 with cost min(1, 0) = 0, now nums = [1,3,3].
    Insert 3 with cost min(1, 0) = 0, now nums = [1,3,3,3].
    Insert 2 with cost min(1, 3) = 1, now nums = [1,2,3,3,3].
    Insert 4 with cost min(5, 0) = 0, now nums = [1,2,3,3,3,4].
    ​​​​​​​Insert 2 with cost min(1, 4) = 1, now nums = [1,2,2,3,3,3,4].
    ​​​​​​​Insert 1 with cost min(0, 6) = 0, now nums = [1,1,2,2,3,3,3,4].
    ​​​​​​​Insert 2 with cost min(2, 4) = 2, now nums = [1,1,2,2,2,3,3,3,4].
    The total cost is 0 + 0 + 0 + 0 + 1 + 0 + 1 + 0 + 2 = 4.

    Constraints:

    - 1 <= instructions.length <= 10^5
    - 1 <= instructions[i] <= 10^5
*/
/*
    Best solution: Fenwick tree (BIT) of counts over the value range

    Complexity:
        Time:  O(n log M) — one point update and two prefix queries per instruction, M = max value (1e5).
        Space: O(M) — the count array of the BIT (or O(n) after coordinate compression).

    Approach (detail):
        1. Keep cnt[v] = how many copies of value v have been inserted so far, stored in a Fenwick tree
           so prefix sums cnt[1..x] cost O(log M).
        2. For instruction x at step i (i elements already inserted):
             less    = prefix(x-1)              — strictly smaller elements,
             greater = i - prefix(x)            — strictly larger elements (equal copies excluded).
        3. Add min(less, greater) to the answer modulo 1e9+7, then insert x (cnt[x] += 1).
        4. Correctness: the cost depends only on the multiset of earlier values, which the BIT holds
           exactly; strict inequalities are what make equal values cost nothing on either side.
           A merge-sort / order-statistics tree gives the same O(n log n) bound; values <= 1e5 make
           the plain BIT the simplest choice.
*/
/*
    Your solution: Fenwick tree of counts indexed directly by value (no compression)

    Complexity:
        Time:  O(n log M) — update + three prefix queries per instruction, M = 1e5.
        Space: O(M) — the `fenwick tree(MAX)` count array.

    Approach:
        1. `fenwick` is a 0-indexed count BIT: update(j) adds 1 at position j, query(j) returns the count
           at positions 0..j (query(-1) = 0).
        2. `no_compression` (the lambda returned) maps value v to position v-1 in a tree of size MAX = 1e5.
        3. For each instruction j it first inserts it (update(j-1)), then
             l = query(j-2)                      — elements strictly less than j,
             r = query(MAX-1) - query(j-1)       — elements strictly greater than j.
           Inserting first is harmless because the new copy is equal to j and is counted in neither.
        4. cost = sum(cost, min(l, r)) adds modulo 1e9+7.

    Also in the file (not called): compression — same loop over coordinate-compressed ranks
    (sort + unique + lower_bound), O(n log n) time and O(n) space.

    Verdict: MATCHES BEST — the live method is the BIT-over-values algorithm above.
        Verified (both variants) on the 3 official examples and 3000 random arrays against an O(n^2)
        brute force.
*/
class Solution {
    public:
        int createSortedArray(vector<int>& instructions) {
            int n = instructions.size();
            // using ii = pair<int, int>;
            // vector<ii> valwindex;
            // for (int i = 0; i < n; ++i){
            //     valwindex.push_back({instructions[i], i});
            // }
            // sort(valwindex.begin(), valwindex.end(), [](const ii& lhs, const ii& rhs){
            //     if (lhs.first < rhs.first) return true;
            //     return (lhs.first == rhs.first && lhs.second < rhs.second);
            // });
            // for (int i = 0; i < n; ++i){
            //     instructions[valwindex[i].second] = i;
            // }
            auto compress = [&](vector<int>& nums){
                vector<int> vals = nums;
                sort(vals.begin(), vals.end());
                vals.erase(unique(vals.begin(), vals.end()), vals.end());
                return vals;
            };

            struct fenwick {
                int N;
                vector<int> bits;

                fenwick(int n) : N(n+1), bits(N, 0){}

                void update(int j){
                    int k = j+1;
                    while (k < N){
                        bits[k] += 1;
                        k += k & (-k);
                    }
                }

                int query(int j){
                    int k = j+1, sum = 0;
                    while (k > 0){
                        sum += bits[k];
                        k -= k & (-k);
                    }
                    return sum;
                }
            };

            const int MOD = 1e9 + 7;
            auto sum = [](int a, int b){
                int c = a + b;
                if (c >= MOD) c -= MOD;
                return c;
            };

            auto compression = [&]() -> int {
                auto vals = compress(instructions);
                auto rank = [&](int x){
                    return lower_bound(vals.begin(), vals.end(), x) - vals.begin();
                };

                fenwick tree(n);
                int cost = 0;
                for (int i = 0; i < n; ++i){
                    //
                    int j = rank(instructions[i]);
                    tree.update(j);
                    int l = tree.query(j-1);
                    int r = tree.query(n-1) - tree.query(j);
                    cost = sum(cost, min(l,r));
                }
                return cost;
            };

            auto no_compression = [&]() -> int {
                const int MAX = 1e5;
                fenwick tree(MAX);
                int cost = 0;
                for (int i = 0; i < n; ++i){
                    int j = instructions[i];
                    tree.update(j-1);
                    int l = tree.query(j-2);
                    int r = tree.query(MAX-1) - tree.query(j-1);
                    cost = sum(cost, min(l,r));
                }
                return cost;
            };
            return no_compression();
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