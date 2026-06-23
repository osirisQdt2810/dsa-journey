#include <vector>
#include <iostream>

using namespace std;

class Solution {
    public:
        long long minimumCost(string source, string target, vector<char>& original, vector<char>& changed, vector<int>& cost) {
            int V = 26;
            int E = original.size();

            // floyd warshall
            using ll = long long;
            ll kNil = 4e18;
            vector<vector<ll>> dist(V, vector<ll>(V, kNil));
            for (int u = 0; u < V; ++u) dist[u][u] = 0;
            for (int e = 0; e < E; ++e){
                int u = original[e] - 'a';
                int v = changed[e] - 'a';
                dist[u][v] = min(dist[u][v], 1LL * cost[e]);
            }

            for (int k = 0; k < V; ++k){
                for (int u = 0; u < V; ++u){
                    for (int v = 0; v < V; ++v){
                        if (dist[u][k] == kNil || dist[k][v] == kNil) continue;
                        dist[u][v] = min(dist[u][v], dist[u][k] + dist[k][v]);
                    }
                }
            }

            int n = source.size();
            ll res = 0;
            for (int i = 0; i < n; ++i){
                ll uv = dist[source[i]-'a'][target[i]-'a'];
                if (uv == kNil) return -1;
                res += uv;
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
    string source = "abcd";
    string target = "abce";
    vector<char> original = {'a'};
    vector<char> changed = {'e'};
    vector<int> cost = {10000};

    long long result = sol.minimumCost(source, target, original, changed, cost);
    cout << "Minimum cost: " << result << endl;

    return 0;
}