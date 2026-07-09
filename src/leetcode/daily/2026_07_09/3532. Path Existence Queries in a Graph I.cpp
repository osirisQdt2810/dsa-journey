#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    public:
        vector<bool> pathExistenceQueries(int n, vector<int>& nums, int maxDiff, vector<vector<int>>& queries) {
            vector<int> lowest_nodes(n, -1);
            for (int i = 0; i < n; ++i){
                lowest_nodes[i] = i;
                if (i > 0 && nums[i] - nums[i-1] <= maxDiff){
                    lowest_nodes[i] = lowest_nodes[i-1];
                }
            }
            vector<bool> res;
            for (auto& query : queries){
                int l = min(query[0], query[1]);
                int r = max(query[0], query[1]);
                res.push_back(l >= lowest_nodes[r]);
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