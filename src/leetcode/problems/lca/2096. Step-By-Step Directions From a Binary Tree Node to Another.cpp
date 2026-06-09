#include <iostream>
#include <vector>

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
        vector<vector<int>> ancestor;   // ancestor[u][i]: tổ tiên thứ 2^i của u
        vector<vector<int>> direction;  // direction[u][i]: direction path from u -> ancestor[u][i], a combination of 0-1, where 0: left, 1: right
                                        // e.g. 010101: to go from u->ancestor[u][i], the path is: uL->uR->uL->uR->uL->uR
        vector<int> h;                  // h[u]: height of u, h[root] = 1
        int n;
        int klog;

        int count_nodes(TreeNode* curr){
            if (!curr) return 0;
            return count_nodes(curr->left) + count_nodes(curr->right);
        }

        void dfs(TreeNode* curr, TreeNode* parent){
            if (!curr) return;
            int u = curr->val;
            int p = parent->val;
            ancestor[u][0] = p;
            for (int j = 1; j < klog; ++j){
                ancestor[u][j] = ancestor[ancestor[u][j-1]][j-1];
            }
            h[u] = h[p] + 1;
            dfs(curr->left, curr);
            dfs(curr->right, curr);
        }

        TreeNode* find_lca(TreeNode* root, int u, int v){
            if (root == nullptr || root->val == u || root->val == v) return root;

            TreeNode* left  = find_lca(root->left, u, v);
            TreeNode* right = find_lca(root->right, u, v);

            // both left & right includes u and v -> root is the lca
            if (left != nullptr && right != nullptr) return root;

            // only left or right includes u and v -> lca is in that child
            return (left) ? left : right;
        }

        TreeNode* dfs(TreeNode* curr, int value, deque<char>& res){
            if (curr == nullptr || curr->val == value) return curr;

            TreeNode* left = dfs(curr->left, value, res);
            TreeNode* right = dfs(curr->right, value, res);
            if (left) res.push_front('L');
            if (right) res.push_front('R');
            return (left) ? left : right;
        }

    public:
        string getDirections(TreeNode* root, int startValue, int destValue) {
            // n = count_nodes(root);
            // klog = floor(log2(n))+1;
            // ancestor.resize(n, vector<int>(klog));
            // dfs(root, root);
            // // lca
            // int u = startValue, v = destValue;
            
            // string res;
            // if (h[u] < h[v]){
            //     // jump v -> v' where h[v'] = h[u]

            // }
            // else {
            //     // jump u -> u' where h[u'] = h[v]
            // }
            TreeNode* lca = find_lca(root, startValue, destValue);
            cout << "u: " << startValue << " v: " << destValue << " lca: " << lca->val << endl;

            deque<char> path2start, path2dest;
            dfs(lca, startValue, path2start);
            dfs(lca, destValue, path2dest);

            string res;
            res.assign(path2start.size(), 'U');
            while (!path2dest.empty()){
                res += path2dest.front();
                path2dest.pop_front();
            }
            return res;
        }
};