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
#include <cmath>
#include <tuple>
#include <functional>

using namespace std;

/*
    LC 3331 - Find Subtree Sizes After Changes
    https://leetcode.com/problems/find-subtree-sizes-after-changes/
    @status: self
    @created: 2026-11-25 15:32

    You are given a tree rooted at node 0 that consists of n nodes numbered from 0 to n - 1. The tree is represented by an array parent of size n, where parent[i] is the parent of node i. Since node 0 is the root, parent[0] == -1.

    You are also given a string s of length n, where s[i] is the character assigned to node i.

    We make the following changes on the tree one time simultaneously for all nodes x from 1 to n - 1:

    - Find the closest node y to node x such that y is an ancestor of x, and s[x] == s[y].
    - If node y does not exist, do nothing.
    - Otherwise, remove the edge between x and its current parent and make node y the new parent of x by adding an edge between them.

    Return an array answer of size n where answer[i] is the size of the subtree rooted at node i in the final tree.

    Example 1:

    Input: parent = [-1,0,0,1,1,1], s = "abaabc"
    Output: [6,3,1,1,1,1]
    Explanation:
    The parent of node 3 will change from node 1 to node 0.

    Example 2:

    Input: parent = [-1,0,4,0,1], s = "abbba"
    Output: [5,2,1,1,1]
    Explanation:
    The following changes will happen at the same time:
    - The parent of node 4 will change from node 1 to node 0.
    - The parent of node 2 will change from node 4 to node 1.

    Constraints:

    - n == parent.length == s.length
    - 1 <= n <= 10^5
    - 0 <= parent[i] <= n - 1 for all i >= 1.
    - parent[0] == -1
    - parent represents a valid tree.
    - s consists only of lowercase English letters.
*/
/*
    Best solution: DFS keeping the closest ancestor per letter, then subtree sizes

    Complexity:
        Time:  O(n + 26) — two linear tree traversals (O(n) with an iterative DFS to avoid deep recursion).
        Space: O(n) — adjacency list, new-parent array and the size array.

    Approach (detail):
        1. All nodes change simultaneously using the ORIGINAL ancestors, so decide every new parent first.
        2. DFS from the root with closest[c] = deepest ancestor on the current root path labelled c.
        3. Entering u: set closest[s[u]] = u (remember the old value), so children see u itself as a candidate.
        4. For each child v: new parent = closest[s[v]] if it exists, else u (unchanged).
        5. Leaving u: restore closest[s[u]] to the saved value.
        6. Rebuild the tree from the new parents and compute subtree sizes with a second DFS (post-order sum).
*/
/*
    Your solution: DFS with closest[] per letter to re-parent, then DFS subtree sizes

    Complexity:
        Time:  O(n) — two DFS passes plus two adjacency builds.
        Space: O(n) — adj, parent/out array reused for sizes; recursion depth O(n).

    Approach:
        1. convert_parent_to_adj builds an undirected adjacency list from parent.
        2. transform_tree(u, p): save tmp = closest[s[u]], set closest[s[u]] = u, set
           out[v] = closest[s[v]] (or u if none) for each child v, recurse, then restore closest.
        3. Rebuild adj from the rewritten parent array.
        4. count_subtree_size(u, p): out[u] = 1 + sum of children sizes, written over the parent array.
        5. Return the array.

    Verdict: MATCHES BEST — same O(n) algorithm (recursive DFS depth up to n = 10^5 is the only caveat).
        Verified against a brute force (climb ancestors for each node, count by walking up) on 30000 random trees, n <= 9.
*/
class Solution {
    public:
        vector<int> findSubtreeSizes(vector<int>& parent, string s) {
            int n = s.size();

            auto convert_parent_to_adj = [](vector<int>& parent, int n){
                vector<vector<int>> adj(n);
                for (int u = 0; u < n; ++u){
                    int p = parent[u];
                    if (p < 0) continue;
                    adj[u].push_back(p);
                    adj[p].push_back(u);
                }
                return adj;
            };

            auto transform_tree = [&](auto&& self, vector<vector<int>>& adj, vector<int>& out, vector<int>& closest, int u, int p) -> void {
                int tmp = closest[s[u]-'a'];
                closest[s[u]-'a'] = u;

                for (auto& v : adj[u]){
                    if (v == p) continue;
                    out[v] = (closest[s[v]-'a'] < 0) ? u : closest[s[v]-'a'];
                    self(self, adj, out, closest, v, u);
                }

                closest[s[u]-'a'] = tmp;
            };

            auto count_subtree_size = [&](auto&& self, vector<vector<int>>& adj, vector<int>& out, int u, int p) -> int {
                out[u] = 1;
                for (auto& v : adj[u]){
                    if (v == p) continue;
                    out[u] += self(self, adj, out, v, u);
                }
                return out[u];
            };

            vector<int> closest(26, -1);
            auto adj = convert_parent_to_adj(parent, n);
            transform_tree(transform_tree, adj, parent, closest, 0, 0);
            adj = convert_parent_to_adj(parent, n);
            count_subtree_size(count_subtree_size, adj, parent, 0, 0);
            return parent;
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
    // test-lc:end
    return 0;
}
