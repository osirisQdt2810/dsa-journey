#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
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
        unordered_map<int, int> depth;

        int count_nodes(TreeNode* curr){
            if (curr == nullptr) return 0;
            return count_nodes(curr->left) + count_nodes(curr->right) + 1;
        }

        void cal_depth(TreeNode* curr, TreeNode* par){
            if (curr == nullptr) return;
            depth[curr->val] = depth[par->val] + 1;
            cal_depth(curr->left, curr);
            cal_depth(curr->right, curr);
        }

        TreeNode* lca(TreeNode* curr, unordered_set<int>& deepest_leaves){
            // Trả về node X trong sub-tree(curr) mà X chứa ít nhất 1 node trong deepest_leaves
            if (curr == nullptr || deepest_leaves.count(curr->val)) return curr;

            TreeNode* left = lca(curr->left, deepest_leaves);
            TreeNode* right = lca(curr->right, deepest_leaves);
            if (left && right) return curr;
            return (left) ? left : right;
        }

    public:
        TreeNode* subtreeWithAllDeepest(TreeNode* root) {
            cal_depth(root, root);
            int max_depth = -1;
            for (auto& [u, d] : depth){
                max_depth = max(d, max_depth);
            }
            unordered_set<int> deepest_leaves;
            for (auto& [u, d] : depth){
                if (d == max_depth){
                    deepest_leaves.insert(u);
                }
            }
            return lca(root, deepest_leaves);
        }
};       
