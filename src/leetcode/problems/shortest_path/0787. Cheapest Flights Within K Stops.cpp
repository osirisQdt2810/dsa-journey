#include <vector>
#include <iostream>

using namespace std;

class Solution {
    private:
        using ll = long long;
        using ii = pair<int, int>;
        struct node {
            int u;
            int edge{0};
            ll cost{0};
        };

        vector<vector<ii>> build_adj(vector<vector<int>>& flights, int n){
            vector<vector<ii>> adj(n);
            for (auto& flight : flights){
                int u = flight[0], v = flight[1], w = flight[2];
                adj[u].push_back({v, w});
            }
            return adj;
        }

    public:
        int findCheapestPrice(int n, vector<vector<int>>& flights, int src, int dst, int k) {
            // thay vì xem bài toán là tìm đường đi ngắn nhất giữa 2 node (u, v)
            // thì ta xem node bây giờ là (u,X) với X là số cạnh đã đi qua (X có thể tăng vô hạn bởi vì có thể có cyclic)
            // rõ ràng đây cũng là 1 bài toán shortest-path vì cạnh không âm
            // ví dụ: đồ thị: 1->2, 1->3, 1->4, 2->3, 2->4, 3->4
            // như vậy bây giờ sẽ có các node:
            // (1, 0) -> (2,1) -> (3,2) -> (4,3)
            //        -> (3,1) -> (4,2)
            //        -> (4,1)
            // thay vì có 4 node thì bây giờ ta sẽ có 7 node, vẫn là bài toán tìm shortest-path từ (1, 0) đến 1 điểm (dest, T) nào đó mà T <= k
            // ta sẽ hạn chế số node bằng cách lọc đi các node mà X > k.
            // rõ ràng, theo thuật dijikstra, mỗi lần pop u thì u là node tốt nhất có cost nhỏ nhất
            // giả sử u = dest => ta có thể kết thúc luôn
            auto adj= build_adj(flights, n);
            auto cmp = [](const node& lhs, const node& rhs){
                if (lhs.cost > rhs.cost) return true;
                return (lhs.cost == rhs.cost && lhs.edge > rhs.edge);
            };
            vector<vector<ll>> dist(n, vector<ll>(k+2, 4e18));  // [k+1]
            priority_queue<node, vector<node>, decltype(cmp)> pq(cmp);
            pq.push({src, 0, 0});
            dist[src][0] = 0;

            while (!pq.empty()){
                node n = pq.top(); pq.pop();
                int u = n.u, edge = n.edge;
                ll cost = n.cost;

                // cout << "u=" << u << " edge: " << edge << " cost: " << cost << endl;
                if (dist[u][edge] != cost) continue;
                if (u == dst) return cost;
                if (edge == k+1) continue;

                for (auto& [v, w] : adj[u]){
                    ll uv = cost+w;
                    if (dist[v][edge+1] > uv){
                        dist[v][edge+1] = uv;
                        pq.push({v, edge+1, uv});
                        // cout << "\t-> push: " << v << " edge: " << edge+1 << " cost: " << uv << endl;
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
    int n = 4;
    vector<vector<int>> flights = {{0,1,100},{1,2,100},{2,0,100},{1,3,600},{2,3,200}};
    int src = 0;
    int dst = 3;
    int k = 1;

    cout << sol.findCheapestPrice(n, flights, src, dst, k);

    return 0;
}