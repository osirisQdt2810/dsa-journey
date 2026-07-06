#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        struct disjointset {
            int n;
            vector<int> parent;
            disjointset(int n) : n(n), parent(n+1, -1e9){}

            int find_root(int u){
                if (parent[u] < 0) return u;
                return parent[u] = find_root(parent[u]);
            }

            void join(int u, int v, int w){
                // cout << "u=" << u << " v=" << v << " w=" << w << "\n";
                int root_u = find_root(u);
                int root_v = find_root(v);
                parent[root_u] = max(parent[root_u], max(parent[root_v], -w));
                if (root_u != root_v) parent[root_v] = root_u;
            }

            int cost(int u){
                return -parent[find_root(u)];
            }

            void print(){
                for (int u = 1; u <= n; ++u){
                    cout << "u=" << u << " p=" << parent[u] << "\n";
                }
            }
        };

        using ii = pair<int, int>;

    public:
        int minScore(int n, vector<vector<int>>& roads) {
            // 1 -> 2 -> 1
            disjointset dsu(n);

            for (auto& road: roads){
                int ai = road[0], bi = road[1], dist = road[2];
                dsu.join(ai, bi, dist);
            }

            // dsu.print();
            return dsu.cost(n);
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