#include <vector>
#include <iostream>

using namespace std;

class Solution {
    public:
        int minTime(int n, vector<vector<int>>& edges) {
            struct neighbor {
                int v;
                int s;
                int e;
            };
            using ii = pair<int, int>;
            vector<vector<neighbor>> adj(n);
            for (auto& edge : edges){
                int u = edge[0], v = edge[1], s = edge[2], e = edge[3];
                adj[u].push_back({v, s, e});
            }

            vector<int> time(n, 2e9);
            priority_queue<ii, vector<ii>, greater<ii>> pq;
            time[0] = 0;
            pq.push({time[0], 0}); 

            while (!pq.empty()){
                auto [tu, u] = pq.top(); pq.pop();
                if (tu != time[u]) continue;
                if (u == n-1) return tu;
                for (auto& nbor : adj[u]){
                    int tv, v = nbor.v, s = nbor.s, e = nbor.e;
                    if (tu <= e){
                        tv = max(tu, s) + 1;
                        if (tv < time[v]){
                            time[v] = tv;
                            pq.push({tv, v});
                        }
                    }
                }
            }
            return -1;
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