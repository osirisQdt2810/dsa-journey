#include <vector>
#include <iostream>

using namespace std;

/*
 * Best solution: DP over time — dp[t][u] = min total passing fee to reach city u having spent
 *                exactly t time units. (A 2D-state Dijkstra over (node, time) also works but is a
 *                log factor slower; see note below.)
 *
 * Time:  O(maxTime * E) — for each time layer t in [0, maxTime] every edge is relaxed once.
 * Space: O(maxTime * n) — the dp table; cannot trivially roll since edge weights vary the t jump.
 *
 * Approach (detail):
 *  1. Edge weights (travel time) are strictly positive, so time only ever increases along a path.
 *     That makes "time" a natural DAG-like layer we can fill in increasing order without cycles.
 *  2. dp[t][u] = cheapest fee sum to stand at city u after exactly t time. Init dp[0][0] = fee[0],
 *     everything else = INF.
 *  3. Sweep t from 0..maxTime. For each undirected edge (u, v, w) with t + w <= maxTime, relax both
 *     directions:
 *        dp[t + w][v] = min(dp[t + w][v], dp[t][u] + fee[v])
 *        dp[t + w][u] = min(dp[t + w][u], dp[t][v] + fee[u])
 *     Because t is processed ascending and w > 0, dp[t][*] is final before it is used as a source.
 *  4. Answer = min over t in [0, maxTime] of dp[t][n-1]; return -1 if it stays INF (unreachable in
 *     time). Minimizing over all t handles "within maxTime", not "exactly maxTime".
 *
 * Note: the code below implements the alternative — Dijkstra popping states by ascending cost with
 *       cost[node][time]; first pop of node n-1 is the answer. Correct, O(maxTime * E * log) time.
 */
class Solution {
    public:
        int minCost(int maxTime, vector<vector<int>>& edges, vector<int>& passingFees) {
            struct state {
                int u;
                int cost;
                int time;

                bool operator<(const state& other) const {
                    return cost > other.cost;
                }
            };
            using ii = pair<int, int>;
            
            int n = passingFees.size();
            vector<vector<ii>> adj(n);
            for (auto& edge : edges){
                int u = edge[0], v = edge[1], t = edge[2];
                adj[u].push_back({v, t});
                adj[v].push_back({u, t});
            }

            const int INF = 1e9;
            priority_queue<state> pq;
            vector<vector<int>> cost(n, vector<int>(maxTime+1, INF));
            pq.push({0, passingFees[0], 0});
            cost[0][0] = passingFees[0];

            while (!pq.empty()){
                auto state = pq.top(); pq.pop();
                int u = state.u, du = state.cost, tu = state.time;
                if (du != cost[u][tu]) continue;
                if (u == n-1) return du;
                for (auto& [v, t] : adj[u]){
                    int tv = tu + t;
                    int dv = du + passingFees[v];
                    if (tv > maxTime) continue;
                    if (dv >= cost[v][tv]) continue;
                    cost[v][tv] = dv;
                    pq.push({v, dv, tv});
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