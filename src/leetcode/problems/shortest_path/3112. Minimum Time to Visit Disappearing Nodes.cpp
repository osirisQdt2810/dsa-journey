#include <vector>
#include <iostream>

using namespace std;

class Solution {
    public:
        vector<int> minimumTime(int n, vector<vector<int>>& edges, vector<int>& disappear) {
            using ii = pair<int, int>;
            vector<vector<ii>> adj(n);
            for (auto& edge : edges){
                int u = edge[0], v = edge[1], w = edge[2];
                adj[u].push_back({v, w});
                adj[v].push_back({u, w});
            }

            vector<int> time(n, -1);
            priority_queue<ii, vector<ii>, greater<ii>> pq;

            time[0] = 0;
            pq.push({time[0], 0});
            while (!pq.empty()){
                auto [tu, u] = pq.top(); pq.pop();
                if (time[u] != tu) continue;
                for (auto& [v, w] : adj[u]){
                    int tv = tu + w;
                    if (tv < disappear[v]){
                        if (time[v] < 0 || tv < time[v]){
                            time[v] = tv;
                            pq.push({time[v], v});
                        }
                    }
                }
            }
            return time;
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