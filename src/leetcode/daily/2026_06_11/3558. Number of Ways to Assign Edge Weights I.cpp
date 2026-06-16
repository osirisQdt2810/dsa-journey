#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        void dfs(int u, int p, vector<vector<int>>& adj, vector<int>& heights){
            heights[u] = heights[p] + 1;
            for (auto& v : adj[u]){
                if (v == p) continue;
                dfs(v, u, adj, heights);
            }
        }

        static inline constexpr int MOD = 1e9 + 7;

    public:
        long long pow(int a, int m){
            if (m == 0) return 1;
            if (m == 1) return a;
            long long sub = pow(a, m/2);
            if (m % 2){
                return (((sub * sub) % MOD) * a) % MOD;
            }
            else {
                return ((sub * sub) % MOD);
            }
        }

        int assignEdgeWeights(vector<vector<int>>& edges) {
            int n = edges.size();
            vector<vector<int>> adj(n+1);
            for (auto& e : edges){
                int u = e[0], v = e[1];
                adj[u].push_back(v);
                adj[v].push_back(u);
            }

            vector<int> heights(n+1, 0);
            dfs(1, 1, adj, heights);
            int max_height = *std::max_element(heights.begin() + 1, heights.end());
            return pow(2, max_height - 1);
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
    vector<int> nums = {1,3,2};
    nums = {4,2,5,1};
    nums = {11,8};
    int k = 2;

    return 0;
}