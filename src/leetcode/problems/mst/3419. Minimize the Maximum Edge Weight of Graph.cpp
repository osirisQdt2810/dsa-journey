#include <vector>
#include <iostream>

using namespace std;

class Solution {
    private:
        using ii = pair<int, int>;
        bool is_valid(vector<vector<ii>>& adj, int threshold, int K){
            int n = adj.size();
            vector<bool> visited(n, false);
            queue<int> q;

            visited[0] = true;
            q.push(0);
            while (!q.empty()){
                for (int i = 0; i < (int)q.size(); ++i){
                    int u = q.front(); q.pop();
                    for (auto& [v, w] : adj[u]){
                        if (w > threshold) continue;
                        if (visited[v]) continue;
                        visited[v] = true;
                        q.push(v);
                    }
                }
            }

            bool res = true;
            for (auto v : visited) res &= v;
            return res;
        }

    public:
        int minMaxWeight(int n, vector<vector<int>>& edges, int threshold) {
            vector<vector<ii>> adj(n);
            for (auto& edge : edges){
                int u = edge[0], v = edge[1], w = edge[2];
                adj[v].push_back({u, w});
            }

            // target = max(weight) in prim:
            // theorem: every Minimum Spanning Tree is a Minimum Bottleneck Spanning Tree
            vector<int> dist(n, INT_MAX);
            vector<int> in_mst(n, false);
            priority_queue<ii, vector<ii>, greater<ii>> pq;

            dist[0] = 0;
            pq.push({dist[0], 0});

            while (!pq.empty()){
                auto [du, u] = pq.top(); pq.pop();
                if (in_mst[u]) continue;
                in_mst[u] = true;

                for (auto& [v, w]: adj[u]){
                    if (!in_mst[v] && w < dist[v]){
                        dist[v] = w;
                        pq.push({dist[v], v});
                    }
                }
            }

            int res = INT_MIN;
            for (auto d : dist){
                if (d == INT_MAX) return -1;
                res = max(d, res);
            }
            return res;
        }

        int minMaxWeightBS_BFS(int n, vector<vector<int>>& edges, int threshold) {
            vector<vector<ii>> adj(n);
            int min_w = INT_MAX, max_w = INT_MIN;
            for (auto& edge : edges){
                int u = edge[0], v = edge[1], w = edge[2];
                adj[v].push_back({u, w});
                min_w = min(min_w, w);
                max_w = max(max_w, w);
            }

            int l = min_w, r = max_w, res = -1;
            while (l <= r){
                int m = (l+r)/2;
                if (is_valid(adj, m, threshold)){
                    res = m;
                    r = m - 1;
                }
                else {
                    l = m + 1;
                }
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