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
    LC 3963 - Create Grid With Exactly One Path
    https://leetcode.com/problems/create-grid-with-exactly-one-path/

    You are given two integers m and n, representing the number of rows and columns of a grid.

    Construct any m x n grid consisting only of the characters '.' and '#', where:

    - '.' represents a free cell.
    - '#' represents an obstacle cell.

    A valid path is a sequence of free cells that:

    - Starts at the top-left cell (0, 0).
    - Ends at the bottom-right cell (m - 1, n - 1).
    - Moves only:
      - Right, from (i, j) to (i, j + 1), or
      - Down, from (i, j) to (i + 1, j).

    Return any grid such that there is exactly one valid path from the top-left cell to the bottom-right cell.

    Example 1:

    Input: m = 2, n = 3
    Output: ["..#","#.."]
    Explanation:
    The only valid path is: (0,0) → (0,1) → (1,1) → (1,2)

    Example 2:

    Input: m = 3, n = 3
    Output: ["..#","#..","##."]
    Explanation:
    The only valid path is: (0,0) → (0,1) → (1,1) → (1,2) → (2,2)

    Example 3:

    Input: m = 1, n = 4
    Output: ["...."]
    Explanation:
    The only valid path is: (0,0) → (0,1) → (0,2) → (0,3)

    Constraints:

    - 1 <= m, n <= 25
*/
/*
    Best solution: constructive "L-shaped" corridor (top row + last column free)

    Complexity:
        Time:  O(m * n) — every cell of the output grid is written once.
        Space: O(1) extra — beyond the m x n output.

    Approach (detail):
        1. Leave row 0 and column n-1 free ('.'), and put '#' in every other cell
           (rows 1..m-1, columns 0..n-2).
        2. From any (0, j) with j < n-1, moving down hits a '#', so the only move is right;
           once the path reaches column n-1, moving right leaves the grid, so the only move is down.
        3. Therefore every step is forced and exactly one path exists:
           (0,0) -> ... -> (0,n-1) -> ... -> (m-1,n-1).
        4. Degenerate shapes need no special case: m = 1 or n = 1 places no '#' and the single
           row/column is itself the unique path.
*/
/*
    Your solution: L-shaped corridor — free top row and last column, block the rest

    Complexity:
        Time:  O(m * n) — builds the grid and fills the blocked rectangle once.
        Space: O(1) extra — only the returned ans.

    Approach:
        1. ans starts as m strings of n '.'.
        2. For i in 1..m-1 and j in 0..n-2, set ans[i][j] = '#', leaving row 0 and column n-1 free.
        3. Return ans: right along row 0, then down column n-1 is the only path.

    Verdict: MATCHES BEST — the same O(m * n) forced-corridor construction.
        Verified by counting monotone paths with a DP for every 1 <= m, n <= 25 (always exactly 1).
*/
class Solution {
    public:
        vector<string> createGrid(int m, int n) {
            vector<string> ans(m, string(n, '.'));
            for (int i = 1; i < m; ++i){
                for (int j = 0; j < n-1; ++j){
                    ans[i][j] = '#';
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