#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

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
class Solution {
public:
    TreeNode* createBinaryTree(vector<vector<int>>& descriptions) {
        struct node {
            TreeNode* addr;
            int indeg{0};
        };

        unordered_map<int, node> val2addr;
        for (auto& desc : descriptions){
            int p = desc[0], c = desc[1], is_left = desc[2];
            if (!val2addr.count(p)) val2addr[p] = {new TreeNode(p), 0};
            if (!val2addr.count(c)) val2addr[c] = {new TreeNode(c), 0};
            (is_left) ? (val2addr[p].addr->left = val2addr[c].addr) : (val2addr[p].addr->right = val2addr[c].addr);
            val2addr[c].indeg++; 
        }

        TreeNode* root = nullptr;
        for (auto& [val, node] : val2addr){
            if (node.indeg == 0){
                root = node.addr;
                break;
            }
        }
        return root;
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