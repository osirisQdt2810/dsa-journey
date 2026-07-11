#include <vector>
#include <iostream>
#include <unordered_set>
#include <map>

using namespace std;

class Solution {
    public:
        // @detail good approach => should be in commit message => treat edge as vertex
        int minimumCost(vector<int>& start, vector<int>& target, vector<vector<int>>& specialRoads) {
            int n = specialRoads.size();
            const int INF = 1e9;

            using point = pair<int, int>;
            using dn = pair<int, int>;      // {dist, node}
            auto point_dist = [](const point& a, const point& b){
                return abs(a.first - b.first) + abs(a.second - b.second);
            };

            // node 0: start, node n+1: target, node v (1..n): special road v-1.
            // standing on a node == being at its exit point (special road is directed).
            point s = {start[0], start[1]};
            point t = {target[0], target[1]};
            auto entry = [&](int u) -> point {
                if (u == 0) return s;
                if (u == n+1) return t;
                auto& r = specialRoads[u-1];
                return {r[0], r[1]};
            };
            auto exit = [&](int u) -> point {
                if (u == 0) return s;
                if (u == n+1) return t;
                auto& r = specialRoads[u-1];
                return {r[2], r[3]};
            };
            auto cost = [&](int u){
                return (u == 0 || u == n+1) ? 0 : specialRoads[u-1][4];
            };

            vector<int> dist(n+2, INF);
            dist[0] = 0;

            priority_queue<dn, vector<dn>, greater<dn>> pq;
            pq.push({dist[0], 0});

            while (!pq.empty()){
                auto [du, u] = pq.top(); pq.pop();
                if (du != dist[u]) continue;
                if (u == n+1) return du;
                point pu = exit(u);                 // where we currently stand
                for (int v = 1; v <= n+1; ++v){     // relax to every special road + target
                    int dv = du + point_dist(pu, entry(v)) + cost(v);
                    if (dv < dist[v]){
                        dist[v] = dv;
                        pq.push({dv, v});
                    }
                }
            }
            return dist[n+1];
        }

        int minimumCost2(vector<int>& start, vector<int>& target, vector<vector<int>>& specialRoads) {
            if (start[0] == target[0] && start[1] == target[1]) return 0;

            using point = pair<int, int>;
            using ii = point;

            auto distance = [](point a, point b){
                return abs(a.first - b.first) + abs(a.second - b.second);
            };

            int cnt = 0;
            map<point, int> pt2id;
            unordered_map<int, vector<ii>> adj;
            point s = {start[0], start[1]};
            point e = {target[0], target[1]};
            pt2id[s] = cnt++;
            pt2id[e] = cnt++;
            int id_s = pt2id[s], id_e = pt2id[e];
            adj[id_s].push_back({id_e, distance(s, e)});
            for (auto& road : specialRoads){
                point a = {road[0], road[1]};
                point b = {road[2], road[3]};
                int dist = road[4];
                if (!pt2id.count(a)) pt2id[a] = cnt++;
                if (!pt2id.count(b)) pt2id[b] = cnt++;
                int id_a = pt2id[a], id_b = pt2id[b];
                if (distance(a, b) > dist){
                    adj[id_a].push_back({id_b, dist});
                    adj[id_s].push_back({id_a, distance(s, a)});
                    adj[id_s].push_back({id_b, distance(s, b)});
                    adj[id_a].push_back({id_e, distance(a, e)});
                    adj[id_b].push_back({id_e, distance(b, e)});
                }
            }

            for (auto& road1 : specialRoads){
                for (auto& road2 : specialRoads){
                    point p_a1 = {road1[0], road1[1]};
                    point p_b1 = {road1[2], road1[3]};
                    point p_a2 = {road2[0], road2[1]};
                    point p_b2 = {road2[2], road2[3]};
                    int a1 = pt2id[p_a1], b1 = pt2id[p_b1];
                    int a2 = pt2id[p_a2], b2 = pt2id[p_b2];
                    adj[a1].push_back({a2, distance(p_a1, p_a2)});
                    adj[a2].push_back({a1, distance(p_a2, p_a1)});
                    adj[a1].push_back({b2, distance(p_a1, p_b2)});
                    adj[b2].push_back({a1, distance(p_b2, p_a1)});
                    adj[a2].push_back({b1, distance(p_a2, p_b1)});
                    adj[b1].push_back({a2, distance(p_b1, p_a2)});
                    adj[a2].push_back({b2, distance(p_a2, p_b2)});
                    adj[b2].push_back({a2, distance(p_b2, p_a2)});
                }
            }

            const int INF = 2e9;
            vector<int> dist(cnt, INF);
            priority_queue<ii, vector<ii>, greater<ii>> pq;
            dist[id_s] = 0;
            pq.push({dist[id_s], id_s});
            while (!pq.empty()){
                auto [du, u] = pq.top(); pq.pop();
                if (dist[u] != du) continue;
                if (u == id_e) return du;
                for (auto& [v, w]: adj[u]){
                    int dv = du + w;
                    if (dv < dist[v]){
                        dist[v] = dv;
                        pq.push({dv, v});
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