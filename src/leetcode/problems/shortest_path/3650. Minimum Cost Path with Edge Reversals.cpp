#include <vector>
#include <iostream>

using namespace std;

class Solution {
    public:
        int minCost(int n, vector<vector<int>>& edges) {
            using ii = pair<int, int>;
            vector<vector<ii>> adj(n);
            for (auto& edge : edges){
                int u = edge[0], v = edge[1], w = edge[2];
                adj[u].push_back({v, w});
                adj[v].push_back({u, 2 * w});
            }
            
            priority_queue<ii, vector<ii>, greater<ii>> pq;
            vector<int> dist(n, INT_MAX);
            dist[0] = 0;
            pq.push({0, 0});
            while (!pq.empty()){
                auto [du, u] = pq.top(); pq.pop();
                if (du != dist[u]) continue;
                if (u == n-1) return du;
                for (auto& [v, w] : adj[u]){
                    int dv = du + w;
                    if (dv < dist[v]){
                        dist[v] = dv;
                        pq.push({dv, v});
                    }
                }
            }
            return -1;
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