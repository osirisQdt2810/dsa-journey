#include <vector>
#include <iostream>

using namespace std;

class Solution {
    public:
        int minCost(vector<vector<int>>& grid) {
            vector<int> dx = {0, 0, 1, -1};
            vector<int> dy = {1, -1, 0, 0};
            const vector<int> COST = {1, 1, 1, 1};

            // dijikstra on weight: 0-1 => BFS 0-1
            int m = grid.size(), n = grid[0].size();
            vector<vector<int>> dist(m, vector<int>(n, 1e9));

            using ii = pair<int, int>;
            deque<ii> q;
            q.push_back({0, 0});
            dist[0][0] = 0;
            while (!q.empty()){
                auto [i, j] = q.front(); q.pop_front();
                if (i == m-1 && j == n-1) return dist[m-1][n-1];

                for (int t = 0; t < 4; ++t){
                    int in = i + dx[t];
                    int jn = j + dy[t];
                    int cn = (grid[i][j] == t + 1) ? 0 : 1;
                    if (in < 0 || in >= m || jn < 0 || jn >= n) continue;
                    int dn = dist[i][j] + cn;
                    if (dist[in][jn] <= dn) continue;
                    dist[in][jn] = dn;
                    if (cn == 0){
                        q.push_front({in, jn});
                    }
                    else {
                        q.push_back({in, jn});
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