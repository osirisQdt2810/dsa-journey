#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;

using ll = long long;
using ii = pair<int, int>;

class Solution {
private:
    struct pqnode {
        int u;
        ll cost{0};
        int consecutive{0};
    };

    vector<vector<ii>> build_adj(int n, vector<vector<int>>& edges, string& labels){
        vector<vector<ii>> adj(n);
        for (auto& edge : edges){
            adj[edge[0]].push_back({edge[1], edge[2]});
        }
        return adj;
    }
    
public:
    int shortestPath(int n, vector<vector<int>>& edges, string labels, int k) {
        auto adj = build_adj(n, edges, labels);
        auto cmp = [](const pqnode& lhs, const pqnode& rhs){
            if (lhs.cost > rhs.cost) return true;
            return (lhs.cost == rhs.cost && lhs.consecutive > rhs.consecutive);
        };
        priority_queue<pqnode, vector<pqnode>, decltype(cmp)> pq(cmp);
        pq.push({0, 0, 1});
        vector<vector<ll>> dist(
            n, vector<ll>(
                51, 4e18
            )
        );
        dist[0][1] = 0;
        while (!pq.empty()){
            auto [u, cost, consecutive] = pq.top(); pq.pop();
            if (dist[u][consecutive] != cost) continue;
            if (u == n-1) {return cost;};
            for (auto& [v, w] : adj[u]){
                int label_v = labels[v] - 'a';
                int label = labels[u] - 'a';
                ll cost_v = cost + w;
                if (label == label_v){
                    if (consecutive + 1 > k) continue;
                    if (dist[v][consecutive+1] > cost_v){
                        dist[v][consecutive+1] = cost_v;
                        pq.push({v, cost_v, consecutive+1});
                    }
                }
                else {
                    if (dist[v][1] > cost_v){
                        dist[v][1] = cost_v;
                        pq.push({v, cost_v, 1});
                    }
                }
            }
        }
        // cout << -1 << "\n";
        return -1;
    }
};