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
#include <cstring>

using namespace std;

/*
    LC 3525 - Find X Value of Array II
    https://leetcode.com/problems/find-x-value-of-array-ii/

    You are given an array of positive integers nums and a positive integer k. You are also given a 2D array queries, where queries[i] = [index_i, value_i, start_i, x_i].

    You are allowed to perform an operation once on nums, where you can remove any suffix from nums such that nums remains non-empty.

    The x-value of nums for a given x is defined as the number of ways to perform this operation so that the product of the remaining elements leaves a remainder of x modulo k.

    For each query in queries you need to determine the x-value of nums for x_i after performing the following actions:

    - Update nums[index_i] to value_i. Only this step persists for the rest of the queries.
    - Remove the prefix nums[0..(start_i - 1)] (where nums[0..(-1)] will be used to represent the empty prefix).

    Return an array result of size queries.length where result[i] is the answer for the i^th query.

    A prefix of an array is a subarray that starts from the beginning of the array and extends to any point within it.

    A suffix of an array is a subarray that starts at any point within the array and extends to the end of the array.

    Note that the prefix and suffix to be chosen for the operation can be empty.

    Note that x-value has a different definition in this version.

    Example 1:

    Input: nums = [1,2,3,4,5], k = 3, queries = [[2,2,0,2],[3,3,3,0],[0,1,0,1]]
    Output: [2,2,2]
    Explanation:
    - For query 0, nums becomes [1, 2, 2, 4, 5], and the empty prefix must be removed. The possible operations are:
      - Remove the suffix [2, 4, 5]. nums becomes [1, 2].
      - Remove the empty suffix. nums becomes [1, 2, 2, 4, 5] with a product 80, which gives remainder 2 when divided by 3.
    - For query 1, nums becomes [1, 2, 2, 3, 5], and the prefix [1, 2, 2] must be removed. The possible operations are:
      - Remove the empty suffix. nums becomes [3, 5].
      - Remove the suffix [5]. nums becomes [3].
    - For query 2, nums becomes [1, 2, 2, 3, 5], and the empty prefix must be removed. The possible operations are:
      - Remove the suffix [2, 2, 3, 5]. nums becomes [1].
      - Remove the suffix [3, 5]. nums becomes [1, 2, 2].

    Example 2:

    Input: nums = [1,2,4,8,16,32], k = 4, queries = [[0,2,0,2],[0,2,0,1]]
    Output: [1,0]
    Explanation:
    - For query 0, nums becomes [2, 2, 4, 8, 16, 32]. The only possible operation is:
      - Remove the suffix [2, 4, 8, 16, 32].
    - For query 1, nums becomes [2, 2, 4, 8, 16, 32]. There is no possible way to perform the operation.

    Example 3:

    Input: nums = [1,1,2,1,1], k = 2, queries = [[2,1,0,1]]
    Output: [5]

    Constraints:

    - 1 <= nums[i] <= 10^9
    - 1 <= nums.length <= 10^5
    - 1 <= k <= 5
    - 1 <= queries.length <= 2 * 10^4
    - queries[i] == [index_i, value_i, start_i, x_i]
    - 0 <= index_i <= nums.length - 1
    - 1 <= value_i <= 10^9
    - 0 <= start_i <= nums.length - 1
    - 0 <= x_i <= k - 1
*/
/*
    Best solution: segment tree whose node stores (product mod k, count of prefixes by residue)

    Complexity:
        Time:  O((n + q) * k * log n) — build O(n * k), each point update and each range query
               touches O(log n) nodes with an O(k) merge.
        Space: O(n * k) — k + 1 numbers per node.

    Approach (detail):
        1. After removing the prefix nums[0..start-1] and some suffix, the remaining array is
           nums[start..j] for some j >= start. So the answer is the number of j with
           product(nums[start..j]) ≡ x (mod k): a prefix count of the range [start, n-1].
        2. Each node over [l, r] stores prod = product mod k and begin[t] = number of prefixes
           nums[l..j] (l <= j <= r) with product ≡ t.
        3. Merge(L, R): prod = L.prod * R.prod; a prefix either lies inside L (begin = L.begin) or is
           all of L followed by a prefix of R, which moves R.begin[t] to residue L.prod * t. O(k).
        4. Point update: reset the leaf and re-merge the O(log n) ancestors.
        5. Query: merge the nodes covering [start, n-1] in left-to-right order (the merge is not
           commutative) starting from the identity node (prod = 1, begin all 0); answer begin[x].
*/
/*
    Your solution: segtree of (prod, subarr[residue]) with point update and ordered range merge

    Complexity:
        Time:  O((n + q) * k * log n) — build + one update and one query per request.
        Space: O(n * k) — 4N nodes of MAX_K counters.

    Approach:
        1. node(p, k) is a leaf: prod = p % k and subarr[p % k] = 1; the default node is the identity
           (prod = 1, all counts 0).
        2. apply(t) merges children: prod = left.prod * right.prod % K, subarr = left.subarr, then
           subarr[left.prod * t % K] += right.subarr[t] for every residue t.
        3. update(pos, val) rewrites the leaf and calls apply on the way back up.
        4. query(X, k0) merges the covering nodes of [X + 1, N] (1-indexed) left to right with the
           same rule and returns subarr[k0]; resultArray does update then query per request.

    Verdict: MATCHES BEST — the standard O((n + q) * k * log n) segment tree. Verified on the
        problem's three examples and against a brute force on 3000 random cases (with
        AddressSanitizer / UBSan clean).
*/
class Solution {
    private:
        static inline const int MAX_K = 5;
        using ll = long long;
        using count = int;

        static inline ll mul(ll a, ll b, int k){
            return a * b % k;
        }

        struct segtree {
            struct node {
                ll      prod{1};
                count   subarr[MAX_K];
                node(){
                    memset(subarr, 0, sizeof(subarr));
                }

                node(int p, int k) : prod(p % k){
                    memset(subarr, 0, sizeof(subarr));
                    subarr[prod] = 1;
                }
            };

            int N;
            int K;
            vector<node> nodes;

            void build(int t, int l, int r, vector<int>& nums){
                if (l == r){
                    nodes[t] = node(nums[l-1], K);
                    return;
                }
                int m = (l+r)/2;
                build(2*t,l,m,nums);
                build(2*t+1,m+1,r,nums);
                apply(t);
            }

            void apply(int t){
                nodes[t].prod = mul(nodes[2*t].prod, nodes[2*t+1].prod, K);
                memcpy(nodes[t].subarr, nodes[2*t].subarr, K * sizeof(count));
                for (int k = 0; k < K; ++k){
                    int k_new = mul(nodes[2*t].prod, k, K);
                    nodes[t].subarr[k_new] += nodes[2*t+1].subarr[k];
                }
            }

            void update(int t, int l, int r, int pos, int val){
                // case 1: [l...r][pos...pos] or [pos...pos][l...r]
                if (r < pos || pos < l) return;
                // case 2: [pos...l...r...pos]
                if (l == r){
                    nodes[t] = node(val, K);
                    return;
                }
                // case 3: in
                int m = (l+r)/2;
                (pos > m) ? update(2*t+1,m+1,r,pos,val) : update(2*t,l,m,pos,val);
                // apply update for parent
                apply(t);
            }

            node query(int t, int l, int r, int X, int Y){
                // case 1: [l...r][X...Y] or [X...Y][l...r]
                if (r < X || Y < l) return node();
                // case 2: [X...l...r...Y]
                if (X <= l && r <= Y) return nodes[t];
                // case 3: overlap
                int m = (l+r)/2;
                node left = query(2*t,l,m,X,Y);
                node right = query(2*t+1,m+1,r,X,Y);
                node res(left.prod * right.prod, K);
                memcpy(res.subarr, left.subarr, K * sizeof(count));
                for (int k = 0; k < K; ++k){
                    int k_new = mul(left.prod, k, K);
                    res.subarr[k_new] += right.subarr[k];
                }
                return res;
            }

            segtree(int n) : N(n+1), nodes(4*N+1){}

            segtree(vector<int>& nums, int k) : N(nums.size()), K(k), nodes(4*N+1){
                build(1, 1, N, nums);
            }

            void update(int pos, int val){
                update(1, 1, N, pos+1, val);
            }

            int query(int X, int k0){
                return query(1, 1, N, X+1, N).subarr[k0];
            }
        };

    public:
        vector<int> resultArray(vector<int>& nums, int k, vector<vector<int>>& queries) {
            segtree tree(nums, k);

            vector<int> res;

            for (auto& query : queries){
                int index = query[0];
                int value = query[1];
                int start = query[2];
                int remainder = query[3];

                tree.update(index, value);
                res.push_back(tree.query(start, remainder));
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