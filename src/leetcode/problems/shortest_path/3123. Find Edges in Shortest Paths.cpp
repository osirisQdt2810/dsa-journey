#include <vector>
#include <iostream>

using namespace std;

class Solution {
    private:
        using ii = pair<int, int>;
        using id = long long;
        using weight = long long;
        using wi = pair<weight, int>;

        static inline const weight oo = 4e18;

        id get_id(int u, int v, int n){
            return u * n + v;
        }

        void collect_traces(vector<vector<int>>& traces, unordered_map<id, int>& id2index, vector<bool>& res, int u, int n){
            if (u == 0) return;
            for (auto& v : traces[u]){
                res[id2index[get_id(u, v, n)]] = true;
                collect_traces(traces, id2index, res, v, n);
            }
        }

        void dijikstra(vector<weight>& dist, vector<vector<ii>>& adj, int source){
            int n = dist.size();

            priority_queue<wi, vector<wi>, greater<wi>> pq;
            dist[source] = 0;
            pq.push({dist[source], source});
            while (!pq.empty()){
                auto [du, u] = pq.top(); pq.pop();
                if (du != dist[u]) continue;
                for (auto& [v, w] : adj[u]){
                    weight dv = du + w;
                    if (dv < dist[v]){
                        dist[v] = dv;
                        pq.push({dv, v});
                    }
                }
            }
        }

    public:
        vector<bool> findAnswer(int n, vector<vector<int>>& edges) {
            const int m = edges.size();
            vector<vector<ii>> adj(n);
            for (int index = 0; index < m; ++index){
                auto& edge = edges[index];
                int u = edge[0], v = edge[1], w = edge[2];
                adj[u].push_back({v, w});
                adj[v].push_back({u, w});
            }

            vector<bool> res(m, false);
            vector<weight> dist_fs(n, oo);
            vector<weight> dist_fe(n, oo);

            dijikstra(dist_fs, adj, 0);
            if (dist_fs[n-1] == oo) return res;
            dijikstra(dist_fe, adj, n-1);

            int sp = dist_fs[n-1];
            for (int e = 0; e < m; ++e){
                auto& edge = edges[e];
                int u = edge[0], v = edge[1], w = edge[2];
                weight new_sp_1 = dist_fs[u] + w + dist_fe[v];
                weight new_sp_2 = dist_fs[v] + w + dist_fe[u];
                res[e] = (sp == new_sp_1) || (sp == new_sp_2);
            }
            return res;
        }

        vector<bool> findAnswer2(int n, vector<vector<int>>& edges) {
            const int m = edges.size();
            vector<vector<int>> traces(n);
            vector<vector<ii>> adj(n);
            unordered_map<id, int> id2index;
            for (int index = 0; index < m; ++index){
                auto& edge = edges[index];
                int u = edge[0], v = edge[1], w = edge[2];
                adj[u].push_back({v, w});
                adj[v].push_back({u, w});
                id2index[get_id(u, v, n)] = index;
                id2index[get_id(v, u, n)] = index;
            }

            vector<bool> res(m, false);
            vector<weight> dist(n, oo);
            priority_queue<wi, vector<wi>, greater<wi>> pq;

            dist[0] = 0;
            pq.push({dist[0], 0});
            while (!pq.empty()){
                auto [du, u] = pq.top(); pq.pop();
                if (du != dist[u]) continue;
                if (u == n-1){
                    // update res
                    collect_traces(traces, id2index, res, u, n);
                    break;
                }
                for (auto& [v, w] : adj[u]){
                    weight dv = du + w;
                    if (dv < dist[v]){
                        dist[v] = dv;
                        pq.push({dv, v});
                        traces[v] = {u};
                    }
                    else if (dv == dist[v]){
                        traces[v].push_back(u);
                    }
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

    int n = 6;
    vector<vector<int>> edges = {{0,1,4},{0,2,1},{1,3,2},{1,4,3},{1,5,1},{2,3,1},{3,5,3},{4,5,2}};

    vector<bool> res = sol.findAnswer(n, edges);

    cout << "\n"; for (auto r : res) cout << r << " ";
    return 0;
}