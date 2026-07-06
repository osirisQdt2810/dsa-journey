#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        static constexpr int mod = (int)1e9 + 7;

        void add(int& a, int b){
            int c = a + b;
            if (c >= mod) c -= mod;
            a = c;
        }

    public:
        vector<int> pathsWithMaxScore(vector<string>& board) {
            int n = board.size();
            struct dpt {
                int sum{0};
                int count{0};
            };

            vector<int> dx = {0, -1, -1};
            vector<int> dy = {-1, 0, -1};

            vector<vector<dpt>> dp(n, vector<dpt>(n));
            for (int i = 0; i < n; ++i){
                for (int j = 0; j < n; ++j){
                    if (board[i][j] == 'X') continue;
                    if (board[i][j] == 'E'){
                        dp[i][j].count = 1;
                        continue;
                    }
                    for (int t = 0; t < 3; ++t){
                        int ip = i + dx[t];
                        int jp = j + dy[t];
                        if (ip < 0 || ip >= n || jp < 0 || jp >= n || board[ip][jp] == 'X') continue;
                        if (!dp[ip][jp].count) continue;
                        int sum_path = dp[ip][jp].sum + (board[i][j] == 'S' ? 0 : (board[i][j] - '0'));
                        if (sum_path > dp[i][j].sum){
                            dp[i][j].sum = sum_path;
                            dp[i][j].count = dp[ip][jp].count;
                        }
                        else if (sum_path == dp[i][j].sum){
                            add(dp[i][j].count, dp[ip][jp].count);
                        }
                    }
                    // cout << "i=" << i << " j=" << j << " dp=" << dp[i][j].sum << " ," << dp[i][j].count << "\n";
                }
            }
            // cout << "\n";
            return {dp[n-1][n-1].sum, dp[n-1][n-1].count};
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

    vector<string> board = {"E23","2X2","12S"};
    for (auto& r : sol.pathsWithMaxScore(board)){
        cout << r << " ";
    }
    return 0;
}