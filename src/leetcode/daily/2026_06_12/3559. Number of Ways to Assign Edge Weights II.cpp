#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        struct LCA {
            int n;
            int klog;

            vector<vector<int>> up; // up[u][j]: 2^j-th ancestor if u
            vector<int> h;          // h[u]: height of u, h[root] = 0

            void dfs(int u, int p, const vector<vector<int>>& adj){
                h[u] = h[p] + 1;
                up[u][0] = p;
                for (int j = 1; j <= klog; ++j)
                    up[u][j] = up[up[u][j-1]][j-1];

                for (auto& v : adj[u]){
                    if (v == p) continue;
                    dfs(v, u, adj);
                }
            }

            LCA(const vector<vector<int>>& adj){
                n = adj.size() + 1;
                klog = floor(log2(n));
                up.resize(n, vector<int>(klog+1, -1));
                h.resize(n, -1);
                dfs(1, 1, adj);
            }

            int lca(int u, int v){
                if (h[u] != h[v]){
                    if (h[u] < h[v]) swap(u, v);
                    int jump = h[u] - h[v];
                    for (int j = 0; j <= log2(jump); ++j){
                        if (1 & (jump >> j)){
                            u = up[u][j];
                        }
                    }
                }
                if (u == v) return u;
                // jump until u and v still not reach their common ancestor
                for (int j = log2(h[u]); j >= 0; --j){
                    if (up[u][j] != up[v][j]){
                        u = up[u][j];
                        v = up[v][j];
                    }
                }
                return up[u][0];
            }

            int distance(int u, int v){
                int lca_v = lca(u, v);
                return h[u] + h[v] - 2*h[lca_v];
            }
        };

    private:
        void dfs(int u, int p, vector<vector<int>>& adj, vector<int>& heights){
            heights[u] = heights[p] + 1;
            for (auto& v : adj[u]){
                if (v == p) continue;
                dfs(v, u, adj, heights);
            }
        }

        static inline constexpr int MOD = 1e9 + 7;
        unordered_map<int, long long> pow_map;

    public:
        long long pow(int a, int m){
            if (pow_map.count(m)) return pow_map[m];
            if (m <  0) return (pow_map[m] = 0);
            if (m == 0) return (pow_map[m] = 1);
            long long sub = pow(a, m/2);
            long long subsqr = (sub * sub) % MOD;
            return pow_map[m] = ((m%2) ? (subsqr * a) % MOD : subsqr);
        }

        vector<int> assignEdgeWeights(vector<vector<int>>& edges, vector<vector<int>>& queries) {
            int v = edges.size();
            int n = v + 1;
            vector<vector<int>> adj(n+1);
            for (auto& e : edges){
                int u = e[0], v = e[1];
                adj[u].push_back(v);
                adj[v].push_back(u);
            }

            vector<int> res;
            LCA lca(adj);
            for (auto& q : queries){
                int d = lca.distance(q[0], q[1]);
                res.push_back(pow(2, d-1));
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
    vector<vector<int>> edges = {{1,2}};
    vector<vector<int>> queries = {{1,1},{1,2}};

    edges = {{1,2},{1,3},{3,4},{3,5}}, queries = {{1,4},{3,4},{2,5}};

    auto res = sol.assignEdgeWeights(edges, queries);
    for (auto& r : res){
        cout << r << " ";
    }

    return 0;
}