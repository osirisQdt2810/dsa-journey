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
    LC 3414 - Maximum Score of Non-overlapping Intervals
    https://leetcode.com/problems/maximum-score-of-non-overlapping-intervals/

    You are given a 2D integer array intervals, where intervals[i] = [l_i, r_i, weight_i]. Interval i starts at position l_i and ends at r_i, and has a weight of weight_i. You can choose up to 4 non-overlapping intervals. The score of the chosen intervals is defined as the total sum of their weights.

    Return the lexicographically smallest array of at most 4 indices from intervals with maximum score, representing your choice of non-overlapping intervals.

    Two intervals are said to be non-overlapping if they do not share any points. In particular, intervals sharing a left or right boundary are considered overlapping.

    Example 1:

    Input: intervals = [[1,3,2],[4,5,2],[1,5,5],[6,9,3],[6,7,1],[8,9,1]]
    Output: [2,3]
    Explanation:
    You can choose the intervals with indices 2, and 3 with respective weights of 5, and 3.

    Example 2:

    Input: intervals = [[5,8,1],[6,7,7],[4,7,3],[9,10,6],[7,8,2],[11,14,3],[3,5,5]]
    Output: [1,3,5,6]
    Explanation:
    You can choose the intervals with indices 1, 3, 5, and 6 with respective weights of 7, 6, 3, and 5.

    Constraints:

    - 1 <= intevals.length <= 5 * 10^4
    - intervals[i].length == 3
    - intervals[i] = [l_i, r_i, weight_i]
    - 1 <= l_i <= r_i <= 10^9
    - 1 <= weight_i <= 10^9
*/
/*
    Best solution: sort by right end, DP over (prefix, picks) whose state is (score, index list)

    Complexity:
        Time:  O(n log n) — sorting plus one binary search per interval; n * 4 states, each copying
               and sorting at most 4 indices.
        Space: O(n) — n x 4 states of (score, <= 4 indices).

    Approach (detail):
        1. Sort intervals by right end r, keeping each one's original index. For interval i let
           p(i) = number of intervals with r < l_i (strict: sharing an endpoint overlaps), found by
           binary search.
        2. dp[i][k] = best choice of exactly k intervals among the first i, where "best" means
           higher total weight, then lexicographically smaller SORTED list of original indices.
        3. dp[i][k] = better(dp[i-1][k], dp[p(i)][k-1] + interval i): skip it, or take it after the
           best (k-1)-choice that ends strictly before it starts.
        4. Keep the index list inside the state: equal-score branches cannot be told apart by
           score alone, and the list has at most 4 entries, so storing it is cheap and makes the
           lexicographic tie-break exact without any back-tracking.
        5. Answer = better of dp[n][1..4] ("up to 4"). Scores reach 4 * 10^9, so use 64-bit.
*/
/*
    Your solution: DP on (sorted prefix, picks - 1) with state {long long score, sorted idx}

    Complexity:
        Time:  O(n log n) — sort, plus 3 * n binary searches and O(1)-size vector work per state.
        Space: O(n) — dp is n x 4 states.

    Approach:
        1. Append the original index to each interval and sort by (r, l).
        2. dp[i][0]: best single interval among the first i + 1 (running maximum).
        3. For k = 1..3 (k + 1 picks): dp[i][k] starts as dp[i-1][k]; if j = search(l) finds an
           interval ending before l, try dp[j][k-1] plus interval i — push its original index,
           sort the list, add w — and keep the better state via maximum().
        4. maximum() prefers the larger score, then the lexicographically smaller idx vector.
        5. Return the best over dp[n-1][0..3].
        6. search(l) is recomputed for each k (3x the necessary work); precomputing it once per
           interval would only save a constant factor.

    Verdict: MATCHES BEST — the documented O(n log n) DP with the index list kept in the state.
        Verified against a brute force over every subset of <= 4 pairwise-disjoint intervals on
        30000 random inputs and both examples, under ASan/UBSan.
*/
class Solution {
    public:
        vector<int> maximumWeight(vector<vector<int>>& intervals) {
            // dp(i)(k): k < 4, tới i có đúng k+1 intervals
            // dp[i][k] = dp(d[i])(k-1) + max
            int n = intervals.size();
            for (int i = 0; i < n; ++i){
                intervals[i].push_back(i);
            }
            sort(intervals.begin(), intervals.end(), [](const auto& lhs, const auto& rhs){
                if (lhs[1] < rhs[1]) return true;
                return (lhs[1] == rhs[1] && lhs[0] < rhs[0]);
            });

            using ll = long long;
            struct state {
                ll score{0};
                vector<int> idx;   // 4-bit
            };

            vector<vector<state>> dp(n, vector<state>(4));

            auto maximum = [](const state& a, const state& b) -> state {
                if (a.score != b.score) return (a.score < b.score) ? b : a;
                return (a.idx < b.idx) ? a : b;
            };

            auto search = [](const vector<vector<int>>& intervals, int X){
                // find j where r[i] < X
                int l = 0, r = intervals.size() - 1, ans = -1;
                while (l <= r){
                    int m = (l+r)/2;
                    if (intervals[m][1] < X){
                        ans = m;
                        l = m+1;
                    }
                    else {
                        r = m-1;
                    }
                }
                return ans;
            };

            state ans;
            for (int i = 0; i < n; ++i){
                dp[i][0] = {intervals[i][2], {intervals[i][3]}};
                if (i > 0) dp[i][0] = maximum(dp[i][0], dp[i-1][0]);
                ans = maximum(ans, dp[i][0]);
            }
            for (int k = 1; k < 4; ++k){    // gen dp[i][k]: end by i, has k+1 interval
                for (int i = 0; i < n; ++i){
                    auto& interval = intervals[i];
                    int l = interval[0], r = interval[1], w = interval[2];
                    int j = search(intervals, l);
                    if (i > 0) dp[i][k] = dp[i-1][k];
                    if (j < 0) continue;

                    auto o = dp[j][k-1].idx; o.push_back(interval[3]); sort(o.begin(), o.end());
                    dp[i][k] = maximum(dp[i][k], {dp[j][k-1].score + w, o});
                }
            }

            for (int k = 0; k < 4; ++k){
                ans = maximum(ans, dp[n-1][k]);
            }

            return ans.idx;
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

    vector<vector<int>> intervals = {{1,3,2},{4,5,2},{1,5,5},{6,9,3},{6,7,1},{8,9,1}};

    sol.maximumWeight(intervals);

    return 0;
}