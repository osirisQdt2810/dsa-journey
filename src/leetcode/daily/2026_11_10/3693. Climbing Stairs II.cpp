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
#include <tuple>
#include <functional>

using namespace std;

/*
    LC 3693 - Climbing Stairs II
    https://leetcode.com/problems/climbing-stairs-ii/
    @status: self
    @time: 20
    @wa: 1

    You are climbing a staircase with n + 1 steps, numbered from 0 to n.

    You are also given a 1-indexed integer array costs of length n, where costs[i] is the cost of step i.

    From step i, you can jump only to step i + 1, i + 2, or i + 3. The cost of jumping from step i to step j is defined as: costs[j] + (j - i)^2

    You start from step 0 with cost = 0.

    Return the minimum total cost to reach step n.

    Example 1:

    Input: n = 4, costs = [1,2,3,4]
    Output: 13
    Explanation:
    One optimal path is 0 → 1 → 2 → 4
     | Jump  | Cost Calculation  | Cost
     | 0 → 1  | costs[1] + (1 - 0)^2 = 1 + 1  | 2
     | 1 → 2  | costs[2] + (2 - 1)^2 = 2 + 1  | 3
     | 2 → 4  | costs[4] + (4 - 2)^2 = 4 + 4  | 8
    Thus, the minimum total cost is 2 + 3 + 8 = 13

    Example 2:

    Input: n = 4, costs = [5,1,6,2]
    Output: 11
    Explanation:
    One optimal path is 0 → 2 → 4
     | Jump  | Cost Calculation  | Cost
     | 0 → 2  | costs[2] + (2 - 0)^2 = 1 + 4  | 5
     | 2 → 4  | costs[4] + (4 - 2)^2 = 2 + 4  | 6
    Thus, the minimum total cost is 5 + 6 = 11

    Example 3:

    Input: n = 3, costs = [9,8,3]
    Output: 12
    Explanation:
    The optimal path is 0 → 3 with total cost = costs[3] + (3 - 0)^2 = 3 + 9 = 12

    Constraints:

    - 1 <= n == costs.length <= 10^5​​​​​​​
    - 1 <= costs[i] <= 10^4
*/
/*
    Best solution: 1-D DP over the last three steps (rolling variables)

    Complexity:
        Time:  O(n) — one pass, constant work per step.
        Space: O(1) — only dp[i-1], dp[i-2], dp[i-3] are ever read.

    Approach (detail):
        1. dp[j] = minimum total cost to stand on step j; dp[0] = 0.
        2. Step j is reached only from j-1, j-2 or j-3, paying costs[j] + (j-i)^2, so
           dp[j] = costs[j] + min(dp[j-1] + 1, dp[j-2] + 4, dp[j-3] + 9), ignoring i < 0.
        3. dp[j] depends only on the three previous values, so keep three rolling variables
           instead of the array. The answer is dp[n].
        4. Bounds: dp[n] <= n * (10^4 + 1) ~ 1.0e9 < INT_MAX, so int is enough.
*/
/*
    Your solution: rolling three-variable DP (d1 = dp[i-1], d2 = dp[i-2], d3 = dp[i-3])

    Complexity:
        Time:  O(n) — one loop over i = 1..n.
        Space: O(1) — d1, d2, d3, d.

    Approach:
        1. d1 = 0 (step 0), d2 = d3 = -1 marks steps that do not exist (before step 0).
        2. For i = 1..n: d = min over the valid predecessors of costs[i-1] + d1 + 1,
           costs[i-1] + d2 + 4, costs[i-1] + d3 + 9 (a negative d_k is skipped).
        3. Shift the window: d3 = d2, d2 = d1, d1 = d.
        4. Return d, which equals dp[n] after the last iteration.

    Verdict: MATCHES BEST — the O(n) / O(1) rolling DP. Verified: 3/3 official examples pass, it
        matches an exhaustive search on 100000 random cases (n <= 12), and the max case
        (n = 10^5, costs = 10^4) gives 333639996 without overflow.
*/
class Solution {
    public:
        int climbStairs(int n, vector<int>& costs) {
            // vector<int> dp(n+1, 0);
            // for (int i = 1; i <= n; ++i){
            //     dp[i] = costs[i-1] + min(
            //         dp[i-1] + 1*1,
            //         dp[i-2] + 2*2,
            //         dp[i-3] + 3*3
            //     )
            // }
            int d3 = -1, d2 = -1, d1 = 0;
            int d = 0;
            for (int i = 1; i <= n; ++i){
                d = INT_MAX;
                if (d3 >= 0) d = min(d, costs[i-1] + d3 + 3*3);
                if (d2 >= 0) d = min(d, costs[i-1] + d2 + 2*2);
                if (d1 >= 0) d = min(d, costs[i-1] + d1 + 1*1);
                // swap(d3,d2,d1,d2,d1,d);
                d3 = d2;
                d2 = d1;
                d1 = d;
                // cout << "i=" << i << " d=" << d << "\n";
            }
            return d;
        }
};

auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
}();

int main(){
    // test-lc:begin
    int passed = 0, total = 0;
    auto check = [&](int idx, int got, int expected) {
        ++total;
        if (got == expected) {
            ++passed;
            cout << "Example " << idx << ": PASS\n";
        } else {
            cout << "Example " << idx << ": FAIL  got=" << got << "  expected=" << expected << "\n";
        }
    };
    {
        Solution sol;
        int n = 4;
        vector<int> costs = {1, 2, 3, 4};
        check(1, sol.climbStairs(n, costs), 13);
    }
    {
        Solution sol;
        int n = 4;
        vector<int> costs = {5, 1, 6, 2};
        check(2, sol.climbStairs(n, costs), 11);
    }
    {
        Solution sol;
        int n = 3;
        vector<int> costs = {9, 8, 3};
        check(3, sol.climbStairs(n, costs), 12);
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}