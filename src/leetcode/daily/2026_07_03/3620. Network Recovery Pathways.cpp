#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        using ll = long long;
        using li = pair<ll, int>;
        using ii = pair<int, int>;
        static constexpr ll oo = 4e18;

        bool is_valid(vector<vector<ii>>& adj, int V, int T, long long k){
            priority_queue<li, vector<li>, greater<li>>pq;
            vector<ll> dist(V, oo);
            pq.push({0, 0});
            dist[0] = 0;

            while (!pq.empty()){
                auto [du, u] = pq.top(); pq.pop();
                if (du != dist[u]) continue;
                if (u == V-1) return du <= k;
                for (auto& [v, w] : adj[u]){
                    ll dv = du + w;
                    if (w < T) continue;
                    if (dv < dist[v]){
                        dist[v] = dv;
                        pq.push({dv, v});
                    }
                }
            }
            return false;

        }
        
    public:
        int findMaxPathScore(vector<vector<int>>& edges, vector<bool>& online, long long k) {
            int min_edge = INT_MAX, max_edge = -INT_MAX;
            for (auto& edge : edges){
                min_edge = min(min_edge, edge[2]);
                max_edge = max(max_edge, edge[2]);
            }
            int l = min_edge, r = max_edge, ans=-1;
            int V = online.size();
            vector<vector<ii>> adj(V);
            for (auto& edge : edges){
                int u = edge[0];
                int v = edge[1];
                int c = edge[2];
                if (!online[u] || !online[v]) continue;
                adj[u].push_back({v, c});
            }

            // F(path) = max(min(path))
            // exist or not a path which min(path) >= T
            // if yes, ans = T => search in [T+1, ]
            // otherwise, search in [, T-1] 
            while (l <= r){
                int m = (l+r)/2;
                if (is_valid(adj, V, m, k)){
                    ans = m;
                    l = m+1;
                }
                else {
                    r = m-1;
                }
            }
            return ans;
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