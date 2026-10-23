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
    LC 1621 - Number of Sets of K Non-Overlapping Line Segments
    https://leetcode.com/problems/number-of-sets-of-k-non-overlapping-line-segments/

    Given n points on a 1-D plane, where the i^th point (from 0 to n-1) is at x = i, find the number of ways we can draw exactly k non-overlapping line segments such that each segment covers two or more points. The endpoints of each segment must have integral coordinates. The k line segments do not have to cover all n points, and they are allowed to share endpoints.

    Return the number of ways we can draw k non-overlapping line segments. Since this number can be huge, return it modulo 10^9 + 7.

    Example 1:

    Input: n = 4, k = 2
    Output: 5
    Explanation: The two line segments are shown in red and blue.
    The image above shows the 5 different ways {(0,2),(2,3)}, {(0,1),(1,3)}, {(0,1),(2,3)}, {(1,2),(2,3)}, {(0,1),(1,2)}.

    Example 2:

    Input: n = 3, k = 1
    Output: 3
    Explanation: The 3 ways are {(0,1)}, {(0,2)}, {(1,2)}.

    Example 3:

    Input: n = 30, k = 7
    Output: 796297179
    Explanation: The total number of possible ways to draw 7 line segments is 3796297200. Taking this number modulo 10^9 + 7 gives us 796297179.

    Constraints:

    - 2 <= n <= 1000
    - 1 <= k <= n-1
*/
/*
    Best solution: closed form C(n + k - 1, 2k) mod 1e9+7

    Complexity:
        Time:  O(n + k) — factorials and inverse factorials up to n + k - 1 (or O(n + k) with a
               single modular inverse via Fermat, O(log MOD)).
        Space: O(n + k) — the factorial tables (O(1) if the binomial is built incrementally).

    Approach (detail):
        1. Segments may share endpoints, which makes the counting awkward. Stretch the line:
           give each of the k segments one extra unit of length, i.e. add k - 1 extra points so
           the n points become n + k - 1 points.
        2. In the stretched line, k segments that may touch become k segments with 2k DISTINCT
           endpoints, and every choice of 2k distinct points (taken in order as l1 < r1 < l2 <
           r2 < ...) gives exactly one valid configuration — a bijection.
        3. So the answer is C(n + k - 1, 2k): compute it with precomputed factorials and modular
           inverses (Fermat's little theorem, MOD is prime).
        4. The O(n * k) DP below (prefix sums over the last segment's start) is the standard
           derivation when the combinatorial bijection is not spotted.
*/
/*
    Your solution: DP over (points, segments) with rolling rows and a running prefix sum

    Complexity:
        Time:  O(n * k) — k layers, each two O(n) passes.
        Space: O(n) — prev, curr and prev_sum rows.

    Approach:
        1. Layer k = 0: prev[n] = 1 (one way to place no segment), prev_sum = its prefix sums.
        2. Layer k: curr[n] = curr[n-1] + prev_sum[n-1] — either point n is not the right end of
           the k-th segment (inherit curr[n-1]), or the k-th segment ends at n and starts at some
           m < n, contributing sum over m < n of prev[m] = prev_sum[n-1] (segments may touch, so
           the (k-1)-th segment may end exactly at m).
        3. Rebuild prev_sum from curr and roll the rows; all additions go through add() mod
           1e9 + 7.
        4. Return prev[N-1].

    Verdict: SUBOPTIMAL — correct, but O(n * k) vs O(n + k) for the closed form C(n + k - 1, 2k);
        at n, k <= 1000 that is ~10^6 steps (~7 ms), well within limits.
        Verified against a brute-force enumeration for n <= 9, the closed form C(n + k - 1, 2k)
        for n up to 1000, and all three examples, under ASan/UBSan.
*/
class Solution {
    private:
        static const inline int MOD = 1e9 + 7;

        int add(int a, int b){
            int c = a + b;
            if (c >= MOD) c -= MOD;
            return c;
        }

    public:
        int numberOfSets(int N, int K) {
            vector<int> prev(N, 0);
            vector<int> prev_sum(N, 0);
            vector<int> curr(N, 0);

            for (int n = 0; n < N; ++n){
                prev[n] = 1;
                prev_sum[n] = ((n > 0) ? prev_sum[n-1] : 0) + prev[n];
            }
            for (int k = 1; k <= K; ++k){
                curr.assign(N, 0);
                for (int n = 1; n < N; ++n){
                    curr[n] = add(curr[n-1], prev_sum[n-1]);
                }

                prev_sum.assign(N, 0);
                for (int n = 1; n < N; ++n){
                    prev_sum[n] = add(prev_sum[n-1], curr[n]);
                }
                swap(prev, curr);
            }
            return prev[N-1];
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
    int n = 4, k = 2;
    n=3;k=1;
    cout << sol.numberOfSets(n, k);
    return 0;
}