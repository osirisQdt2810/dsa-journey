#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
public:
    int countCompleteComponents(int n, vector<vector<int>>& edges) {
        struct DisjointSetUnion {
            vector<int> parents;
            vector<int> num_edges;
        
            DisjointSetUnion(int n) : parents(n, -1), num_edges(n, 0){}
        
            int find_root(int u){
                return(parents[u] < 0) ? u : (parents[u] = find_root(parents[u]));
            }

            void join(int u, int v){
                int root_u = find_root(u);
                int root_v = find_root(v);
                num_edges[root_u] += 1;
                if (root_u == root_v) return;
                parents[root_u] += parents[root_v];
                parents[root_v] = root_u;
                num_edges[root_u] += num_edges[root_v];
            }

            int components(){
                int c = 0;
                for (int u = 0; u < (int)parents.size(); ++u){
                    if (parents[u] < 0){
                        int v = -parents[u];
                        int e = num_edges[u];
                        c += (e == v * (v-1) / 2);
                    }
                }
                return c;
            }
        };

        DisjointSetUnion dsu(n);
        for (auto& edge : edges){
            dsu.join(edge[0], edge[1]);
        }
        return dsu.components();
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

    vector<int> nums = {1,8,3,4,2};
    int maxDiff = 3;
    int n = 5;
    vector<vector<int>> queries = {{0,3}, {2,4}};
    
    auto res = sol.pathExistenceQueries(n, nums, maxDiff, queries);
    cout << "\n"; for (auto r : res) cout << r << " ";
    return 0;
}