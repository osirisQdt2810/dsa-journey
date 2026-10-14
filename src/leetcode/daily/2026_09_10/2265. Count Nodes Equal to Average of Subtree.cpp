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
    LC 2265 - Count Nodes Equal to Average of Subtree
    https://leetcode.com/problems/count-nodes-equal-to-average-of-subtree/

    Problem:
        Given the root of a binary tree, return the number of nodes where the value of the node is
        equal to the average of the values in its subtree.
        - The average of n elements is the sum of the n elements divided by n and rounded down to the
          nearest integer.
        - A subtree of root is a tree consisting of root and all of its descendants.

        Constraints:
            The number of nodes in the tree is in the range [1, 1000].
            0 <= Node.val <= 1000

    Examples:
        Input: root = [4,8,5,0,1,null,6]
        Output: 5
        Explanation: node 4: 24 / 6 = 4; node 5: 11 / 2 = 5; nodes 0, 1, 6 are leaves.

        Input: root = [1]
        Output: 1

    Best solution: single post-order DFS returning (subtree sum, subtree size, count) per node.

    Complexity:
        Time:  O(n) — every node is visited once and does O(1) work.
        Space: O(h) — recursion depth, h = tree height (O(n) worst case for a skewed tree).

    Approach (detail):
        1. dfs(node) returns {sum, nodes, ans} for the subtree rooted at node; a null child is {0,0,0}.
        2. Combine children: nodes = l.nodes + r.nodes + 1, sum = val + l.sum + r.sum.
        3. The node qualifies iff val == sum / nodes (integer division = rounded-down average);
           ans = l.ans + r.ans + (qualifies).
        4. Post-order is needed because a node's average depends on its whole subtree, which is only
           known after both children return. Sums fit in int (<= 1000 * 1000).

    Your solution:
        MATCHES BEST — exactly the O(n) post-order DFS aggregating (sum, nodes, ans).
*/

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
    private:
        struct ret {
            int sum{0};
            int nodes{0};
            int ans{0};
        };

        ret dfs(TreeNode* root){
            ret res;
            if (root == nullptr) return res;
            ret left = dfs(root->left);
            ret right = dfs(root->right);
            res.nodes = left.nodes + right.nodes + 1;
            res.sum = root->val + left.sum + right.sum;
            res.ans = left.ans + right.ans + (root->val == res.sum / res.nodes);
            return res;
        }

    public:
        int averageOfSubtree(TreeNode* root) {
            return dfs(root).ans;
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