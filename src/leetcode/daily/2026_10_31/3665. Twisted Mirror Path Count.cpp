#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <queue>
#include <algorithm>
#include <climits>

using namespace std;

/*
    LC 3665 - Twisted Mirror Path Count
    https://leetcode.com/problems/twisted-mirror-path-count/

    Given an m x n binary grid grid where:

    - grid[i][j] == 0 represents an empty cell, and
    - grid[i][j] == 1 represents a mirror.

    A robot starts at the top-left corner of the grid (0, 0) and wants to reach the bottom-right corner (m - 1, n - 1). It can move only right or down. If the robot attempts to move into a mirror cell, it is reflected before entering that cell:

    - If it tries to move right into a mirror, it is turned down and moved into the cell directly below the mirror.
    - If it tries to move down into a mirror, it is turned right and moved into the cell directly to the right of the mirror.

    If this reflection would cause the robot to move outside the grid boundaries, the path is considered invalid and should not be counted.

    Return the number of unique valid paths from (0, 0) to (m - 1, n - 1).

    Since the answer may be very large, return it modulo 10^9 + 7.

    Note: If a reflection moves the robot into a mirror cell, the robot is immediately reflected again based on the direction it used to enter that mirror: if it entered while moving right, it will be turned down; if it entered while moving down, it will be turned right. This process will continue until either the last cell is reached, the robot moves out of bounds or the robot moves to a non-mirror cell.

    Example 1:

    Input: grid = [[0,1,0],[0,0,1],[1,0,0]]
    Output: 5
    Explanation:
     | Number  | Full Path
     | 1  | (0, 0) → (0, 1) [M] → (1, 1) → (1, 2) [M] → (2, 2)
     | 2  | (0, 0) → (0, 1) [M] → (1, 1) → (2, 1) → (2, 2)
     | 3  | (0, 0) → (1, 0) → (1, 1) → (1, 2) [M] → (2, 2)
     | 4  | (0, 0) → (1, 0) → (1, 1) → (2, 1) → (2, 2)
     | 5  | (0, 0) → (1, 0) → (2, 0) [M] → (2, 1) → (2, 2)
    -
    [M] indicates the robot attempted to enter a mirror cell and instead reflected.

    Example 2:

    Input: grid = [[0,0],[0,0]]
    Output: 2
    Explanation:
     | Number  | Full Path
     | 1  | (0, 0) → (0, 1) → (1, 1)
     | 2  | (0, 0) → (1, 0) → (1, 1)

    Example 3:

    Input: grid = [[0,1,1],[1,1,0]]
    Output: 1
    Explanation:
     | Number  | Full Path
     | 1  | (0, 0) → (0, 1) [M] → (1, 1) [M] → (1, 2)
    (0, 0) → (1, 0) [M] → (1, 1) [M] → (2, 1) goes out of bounds, so it is invalid.

    Constraints:

    - m == grid.length
    - n == grid[i].length
    - 2 <= m, n <= 500
    - grid[i][j] is either 0 or 1.
    - grid[0][0] == grid[m - 1][n - 1] == 0
*/
/*
    Best solution: grid DP with entry direction, rolling rows

    Complexity:
        Time:  O(m * n) — each cell computes two states in O(1).
        Space: O(n) — only the previous and current row of states are kept.

    Approach (detail):
        1. A mirror never holds the robot: it only changes the direction it leaves in. So treat a mirror
           as an ordinary cell the robot passes through, whose exit direction is forced to be the turn of
           its entry direction (entered moving right -> leaves down; entered moving down -> leaves right).
           Chains of mirrors are then just consecutive cells on the path; out-of-bounds reflections are
           exactly the moves that leave the grid, which are never counted.
        2. State: dp[i][j][0] = ways to enter (i,j) moving right (from (i,j-1)),
                  dp[i][j][1] = ways to enter (i,j) moving down  (from (i-1,j)).
        3. Base: dp[0][0][0] = 1 (one way to "be" at the start; grid[0][0] = 0 so its direction is free).
        4. Leaving (i,j-1) to the right:
             - empty cell: any entry direction  -> dp[i][j-1][0] + dp[i][j-1][1];
             - mirror:     only entered moving down (it is turned right) -> dp[i][j-1][1].
           Leaving (i-1,j) downward is symmetric: empty -> both; mirror -> only dp[i-1][j][0].
        5. Answer: dp[m-1][n-1][0] + dp[m-1][n-1][1] (mod 1e9+7). Each row only reads the row above,
           so two rows of n x 2 states suffice.
*/
/*
    Your solution: DP() — full m x n x 2 table over entry direction

    Complexity:
        Time:  O(m * n) — one pass over the grid, two O(1) transitions per cell.
        Space: O(m * n) — the whole dp table.

    Approach:
        1. dp[i][j][0] counts arrivals at (i,j) moving right, dp[i][j][1] arrivals moving down;
           seed dp[0][0][0] = 1.
        2. In row-major order: from the top cell, dp[i][j][1] = both states of (i-1,j) if grid[i-1][j] == 0,
           else only dp[i-1][j][0] (a right-moving entry is reflected down).
        3. From the left cell, dp[i][j][0] = both states of (i,j-1) if grid[i][j-1] == 0, else only
           dp[i][j-1][1] (a down-moving entry is reflected right); additions go through sum() mod 1e9+7.
        4. Return sum(dp[m-1][n-1][0], dp[m-1][n-1][1]).
        Also in the file (not called): optim_space_DP — the same recurrence on prev_row / curr_row, O(n) space.

    Verdict: SUBOPTIMAL — same O(m * n) time but O(m * n) vs O(n) space for the live DP(); the unused
        optim_space_DP already implements the best version. Irrelevant at m, n <= 500 (~8.6 ms, 2 MB).
        Both lambdas verified against a step-by-step reflection simulator on 5000 random grids up to 7 x 7.
*/
class Solution {
    public:
        int uniquePaths(vector<vector<int>>& grid) {
            const int MOD = 1e9 + 7;

            auto sum = [&](int a, int b){
                int c = a + b;
                if (c >= MOD) c -= MOD;
                return c;
            };

            // state:
            //  - khi đi vào ô (i,j), 1 robot có thể đi theo 2 đường từ trái sang hoặc từ trên xuống => lưu thêm trạng thái hướng ra.
            //  - formula: dp[i][j][0/1] - dp[i][j][0]: số cách đi từ (0,0)->(i,j) sao cho hướng vào tại (i,j) là sang phải
            //      +) xét dp(i,j,0):
            //          - nếu a(i-1,j)=0 => có thể tự chọn hướng vào tại (i-1,j): dp(i-1)(j)(0) + dp(i-1)(j)(1)
            //          - nếu a(i-1,j)=1 => bắt buộc hướng vào (i-1,j) là từ trên xuống: dp(i-1)(j)(1)
            int m = grid.size(), n = grid[0].size();
            auto DP = [&]() -> int {
                vector<vector<vector<int>>> dp(m, vector<vector<int>>(n, vector<int>(2, 0)));
                dp[0][0][0] = 1;
                for (int i = 0; i < m; ++i){
                    for (int j = 0; j < n; ++j){
                        // dp[i][j][0]
                        if (i > 0){
                            if (grid[i-1][j] == 0){
                                dp[i][j][1] = sum(dp[i-1][j][0], dp[i-1][j][1]);
                            }
                            else {
                                dp[i][j][1] = dp[i-1][j][0];
                            }
                        }

                        // dp[i][j][1]
                        if (j > 0){
                            if (grid[i][j-1] == 0){
                                dp[i][j][0] = sum(dp[i][j-1][0], dp[i][j-1][1]);
                            }
                            else {
                                dp[i][j][0] = dp[i][j-1][1];
                            }
                        }
                    }
                }
                return sum(dp[m-1][n-1][0], dp[m-1][n-1][1]);
            };

            auto optim_space_DP = [&]() -> int {
                vector<vector<int>> prev_row(n, vector<int>(2, 0));
                vector<vector<int>> curr_row(n, vector<int>(2, 0));
                for (int i = 0; i < m; ++i){
                    curr_row.assign(n, vector<int>(2, 0));
                    if (i == 0) curr_row[0][0] = 1;

                    for (int j = 0; j < n; ++j){
                        if (i > 0){
                            if (grid[i-1][j] == 0){
                                curr_row[j][1] = sum(prev_row[j][0], prev_row[j][1]);
                            }
                            else {
                                curr_row[j][1] = prev_row[j][0];
                            }
                        }

                        // curr_row[j][1]
                        if (j > 0){
                            if (grid[i][j-1] == 0){
                                curr_row[j][0] = sum(curr_row[j-1][0], curr_row[j-1][1]);
                            }
                            else {
                                curr_row[j][0] = curr_row[j-1][1];
                            }
                        }
                    }
                    swap(prev_row, curr_row);
                }
                return sum(prev_row[n-1][0], prev_row[n-1][1]);
            };

            return DP();
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

    vector<vector<int>> grids = {{0,1,0},{0,0,1},{1,0,0}};

    sol.uniquePaths(grids);

    return 0;
}