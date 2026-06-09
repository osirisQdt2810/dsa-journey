#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

class Solution {
    private:
        using AdjMat = vector<vector<pair<int, int>>>;

        struct LCA {
            struct node {
                int v{-1};
                // int dist{0};
                int occurences[27];
            };

            vector<vector<node>> ancestor;   // ancestor[u][i]: tổ tiên thứ 2^i của u và tập hơn các thông tin từ u -> ancestor[u][i]]
            vector<int> h;                   // h[u]: height of u, h[root] = 1;
            int n;
            int klog;

            void _sum_wi(int* occurences_A, int* occurences_B, int* occurences_C, bool accum=false){
                for (int wi = 1; wi <= 26; ++wi){
                    if (accum){
                        occurences_C[wi] += occurences_A[wi] + occurences_B[wi];
                    } else {
                        occurences_C[wi] = occurences_A[wi] + occurences_B[wi];
                    }
                }
            }

            void dfs(const AdjMat& mat, int u, int p, int w){
                ancestor[u][0].v = p;
                // ancestor[u][0].dist = (u != p);
                ancestor[u][0].occurences[w]++;

                for (int j = 1; j <= klog; ++j){
                    int k = ancestor[u][j-1].v;
                    ancestor[u][j].v    = ancestor[k][j-1].v;
                    // ancestor[u][j].dist = ancestor[u][j-1].dist + ancestor[k][j-1].dist;
                    _sum_wi(ancestor[u][j-1].occurences, ancestor[k][j-1].occurences, ancestor[u][j].occurences);
                }

                h[u] = h[p] + 1;
                for (auto& [v, ew] : mat[u]){
                    if (v == p) continue;
                    dfs(mat, v, u, ew);
                }
            }

            LCA(const AdjMat& mat, int n) : n(n), klog(floor(log2(n))){
                ancestor.resize(n, vector<node>(klog+1));
                h.resize(n, 0);
                dfs(mat, 0, 0, 0);
            }

            int query(int u, int v){
                // cout << "u=" << u << " v=" << v << endl;
                int occurences[27]; memset(occurences, 0, sizeof(occurences));
                int dist=0;
                int u_ = u, v_ = v;
                
                if (h[u] != h[v]){
                    if (h[u] < h[v]) swap(u, v);
                    int distance = h[u] - h[v];
                    // jump u -> u' (via distance) to get h[u] = h[u'] = h[v]
                    for (int j = 0; j <= log2(distance); ++j){
                        if ((distance >> j) & 1){
                            _sum_wi(occurences, ancestor[u][j].occurences, occurences);
                            u = ancestor[u][j].v;
                            // cout << "\tjump u=" << u << " distance=" << ancestor[u][j].dist << endl;
                            // for (int i = 1; i <= 26; ++i){
                            //     cout << occurences[i] << " ";
                            // }
                            // cout << endl;
                        }
                    }
                    // dist += distance;
                }
                int lca = u;
                if (u != v){
                    for (int j = klog; j >= 0; j--){
                        if (ancestor[u][j].v != ancestor[v][j].v){
                            // dist += ancestor[u][j].dist + ancestor[v][j].dist;
                            _sum_wi(ancestor[u][j].occurences, ancestor[v][j].occurences, occurences, true);
                            u = ancestor[u][j].v;
                            v = ancestor[v][j].v;
                        }
                    }
                    // dist += ancestor[u][0].dist + ancestor[v][0].dist;
                    _sum_wi(ancestor[u][0].occurences, ancestor[v][0].occurences, occurences, true);
                    lca = ancestor[u][0].v;
                }
                int max_occurences = *std::max_element(occurences+1, occurences + 27);
                dist = h[u_] + h[v_] - 2 * h[lca];
                return dist - max_occurences;
            }
        };

        AdjMat build_adjacent_matrix(int n, vector<vector<int>>& edges){
            AdjMat mat(n);
            for (auto& edge : edges){
                int u = edge[0], v = edge[1], w = edge[2];
                mat[u].push_back({v, w});
                mat[v].push_back({u, w});
            }
            return mat;
        }

        LCA build_lca(int n, const AdjMat& mat){
            return LCA(mat, n);
        } 

    public:
        vector<int> minOperationsQueries(int n, vector<vector<int>>& edges, vector<vector<int>>& queries) {
            auto adj = build_adjacent_matrix(n, edges);
            auto lca = build_lca(n, adj);
            vector<int> res;
            for (auto& query: queries){
                int a = query[0], b = query[1];
                res.push_back(lca.query(a,b));
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

    int n = 7;
    vector<vector<int>> edges = {{0,1,1},{1,2,1},{2,3,1},{3,4,2},{4,5,2},{5,6,2}};
    vector<vector<int>> queries = {{0,3},{3,6},{2,6},{0,6}};

    auto res = sol.minOperationsQueries(n, edges, queries);
    for (auto& r : res) cout << r << "\t";
    return 0;
}