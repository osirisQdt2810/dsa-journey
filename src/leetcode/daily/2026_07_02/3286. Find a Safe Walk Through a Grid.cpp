#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        using ii = pair<int, int>;
        const int dx[4] = {1, -1, 0, 0};
        const int dy[4] = {0, 0, 1, -1};

        bool is_good(vector<vector<int>> dist, int DIST, int n){
            if (dist[0][0] < DIST) return false;
            queue<ii> q;
            q.push({0, 0});
            vector<vector<bool>> visited(n, vector<bool>(n, false));
            visited[0][0] = true;
            while (!q.empty()){
                auto [i, j] = q.front(); q.pop();
                if (i == n-1 && j == n-1) return true;
                for (int t = 0; t < 4; ++t){
                    int in = i + dx[t];
                    int jn = j + dy[t];
                    if (in < 0 || in >= n || jn < 0 || jn >= n || visited[in][jn]) continue;
                    if (dist[in][jn] < DIST) continue;
                    visited[in][jn] = true;
                    q.push({in, jn});
                }
            }
            return false;
        }

    public:
        int maximumSafenessFactor(vector<vector<int>>& grid) {
            // max(F)
            // tồn tại 1 đường mà min(path) >= X => all path >= X
            int n = grid.size();
            if (grid[0][0] == 1) return 0;
            vector<vector<int>> dist(n, vector<int>(n, -1));
            queue<ii> q;
            for (int i = 0; i < n; ++i){
                for (int j = 0; j < n; ++j){
                    if (grid[i][j] == 1){
                        dist[i][j] = 0;
                        q.push({i, j});
                    }
                }
            }
            while (!q.empty()){
                int l = q.size();
                while (l--){
                    auto [i, j] = q.front(); q.pop();
                    for (int t = 0; t < 4; ++t){
                        int in = i + dx[t];
                        int jn = j + dy[t];
                        if (in < 0 || in >= n || jn < 0 || jn >= n || dist[in][jn] >= 0) continue;
                        dist[in][jn] = dist[i][j] + 1;
                        q.push({in, jn});
                    }
                }
            }

            int l = 0, r = n, ans = 0;
            while (l <= r){
                int m = (l+r)/2;
                if (is_good(dist, m, n)){
                    ans = m;
                    l = m+1;
                }
                else {
                    r = m-1;
                }
            }
            return ans;
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