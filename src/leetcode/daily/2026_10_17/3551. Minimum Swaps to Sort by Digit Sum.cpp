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
    LC 3551 - Minimum Swaps to Sort by Digit Sum
    https://leetcode.com/problems/minimum-swaps-to-sort-by-digit-sum/

    You are given an array nums of distinct positive integers. You need to sort the array in increasing order based on the sum of the digits of each number. If two numbers have the same digit sum, the smaller number appears first in the sorted order.

    Return the minimum number of swaps required to rearrange nums into this sorted order.

    A swap is defined as exchanging the values at two distinct positions in the array.

    Example 1:

    Input: nums = [37,100]
    Output: 1
    Explanation:
    - Compute the digit sum for each integer: [3 + 7 = 10, 1 + 0 + 0 = 1] → [10, 1]
    - Sort the integers based on digit sum: [100, 37]. Swap 37 with 100 to obtain the sorted order.
    - Thus, the minimum number of swaps required to rearrange nums is 1.

    Example 2:

    Input: nums = [22,14,33,7]
    Output: 0
    Explanation:
    - Compute the digit sum for each integer: [2 + 2 = 4, 1 + 4 = 5, 3 + 3 = 6, 7 = 7] → [4, 5, 6, 7]
    - Sort the integers based on digit sum: [22, 14, 33, 7]. The array is already sorted.
    - Thus, the minimum number of swaps required to rearrange nums is 0.

    Example 3:

    Input: nums = [18,43,34,16]
    Output: 2
    Explanation:
    - Compute the digit sum for each integer: [1 + 8 = 9, 4 + 3 = 7, 3 + 4 = 7, 1 + 6 = 7] → [9, 7, 7, 7]
    - Sort the integers based on digit sum: [16, 34, 43, 18]. Swap 18 with 16, and swap 43 with 34 to obtain the sorted order.
    - Thus, the minimum number of swaps required to rearrange nums is 2.

    Constraints:

    - 1 <= nums.length <= 10^5
    - 1 <= nums[i] <= 10^9
    - nums consists of distinct positive integers.
*/
/*
    Best solution: sort by (digit sum, value), then count cycles of the induced permutation

    Complexity:
        Time:  O(n log n) — the sort dominates; digit sums cost O(log10 C) each and the cycle walk is O(n).
        Space: O(n) — the (key, index) array and a visited marker.

    Approach (detail):
        1. The target order is fixed: sort by digit sum, ties by value (all values are distinct, so
           the order is a total order and each element has exactly one target position).
        2. Pair every element with its original index and sort; position p of the sorted array then
           says which original index must end up at p — a permutation of 0..n-1.
        3. The minimum number of swaps that sorts a permutation is n - (number of cycles): each swap
           can increase the cycle count by at most one, and fixing one element per swap inside a
           cycle of length L takes exactly L - 1 swaps.
        4. Walk each unvisited position along p -> sorted[p].index until returning, counting cycles.
*/
/*
    Your solution: sort (digit sum, index) pairs with a value tie-break, then count permutation cycles

    Complexity:
        Time:  O(n log n) — the sort; digits() is O(10) per value and the cycle walk is O(n).
        Space: O(n) — arr and visit.

    Approach:
        1. arr[i] = {digits(nums[i]), i}; sort by digit sum, ties broken by nums[index].
        2. visit[] marks positions; for each unvisited u, follow v = arr[v].second until a visited
           position is reached, counting one cycle per start.
        3. Return n - cycles.

    Verdict: MATCHES BEST — the O(n log n) sort + cycle count. Verified against a BFS over all swap
        sequences on 3000 random arrays with n <= 6.
*/
class Solution {
    private:
        int digits(int k){
            int r = 0;
            while (k > 0){
                r += k % 10;
                k /= 10;
            }
            return r;
        }

    public:
        int minSwaps(vector<int>& nums) {
            int n = nums.size();
            using ii = pair<int, int>;
            vector<ii> arr;
            for (int i = 0; i < n; ++i){
                arr.push_back({digits(nums[i]), i});
            }
            sort(arr.begin(), arr.end(), [&](const auto& lhs, const auto& rhs) -> bool {
                if (lhs.first < rhs.first) return true;
                if (lhs.first == rhs.first) return (nums[lhs.second] < nums[rhs.second]);
                return false;
            });
            vector<bool> visit(n, false);
            int cycles = 0;
            for (int u = 0; u < n; ++u){
                if (!visit[u]){
                    ++cycles;

                    int v = u;
                    while (!visit[v]){
                        visit[v] = true;
                        v = arr[v].second;
                    }
                }
            }
            return n - cycles;
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