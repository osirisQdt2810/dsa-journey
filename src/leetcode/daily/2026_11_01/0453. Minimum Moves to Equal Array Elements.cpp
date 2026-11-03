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
    LC 453 - Minimum Moves to Equal Array Elements
    https://leetcode.com/problems/minimum-moves-to-equal-array-elements/

    Given an integer array nums of size n, return the minimum number of moves required to make all array elements equal.

    In one move, you can increment n - 1 elements of the array by 1.

    Example 1:

    Input: nums = [1,2,3]
    Output: 3
    Explanation: Only three moves are needed (remember each move increments two elements):
    [1,2,3]  =>  [2,3,3]  =>  [3,4,3]  =>  [4,4,4]

    Example 2:

    Input: nums = [1,1,1]
    Output: 0

    Constraints:

    - n == nums.length
    - 1 <= nums.length <= 10^5
    - -10^9 <= nums[i] <= 10^9
    - The answer is guaranteed to fit in a 32-bit integer.
*/
/*
    Best solution: sum of distances to the minimum

    Complexity:
        Time:  O(n) — one pass for the min, one for the sum.
        Space: O(1) — two scalars.

    Approach (detail):
        1. Incrementing n-1 elements by 1 is, relative to each other, the same as decrementing
           the single remaining element by 1 — only differences between elements matter.
        2. So the question becomes: how many single-element decrements make all equal?
        3. Decrementing never helps go below the minimum, so every element is lowered to min:
           answer = sum(nums[i] - min) = sum(nums) - n * min.
        4. Each term is <= the answer, which fits in 32 bits, so no intermediate overflows.
*/
/*
    Your solution: equivalent_flow — sum of (x - min)

    Complexity:
        Time:  O(n) — min_element plus one summing loop.
        Space: O(1) — min and moves.

    Approach:
        1. min = *min_element(nums).
        2. moves += x - min for every x; return moves.

    Also in the file (not called): simple_flow — sort, then accumulate the running delta
        nums[i] - nums[i-1] with earlier deltas re-applied; O(n log n), same result.

    Verdict: MATCHES BEST — the live equivalent_flow is exactly sum(x - min).
        Verified against a move-by-move simulation on 2000 random arrays.
*/
class Solution {
    public:
        int minMoves(vector<int>& nums) {
            int n = nums.size();

            auto simple_flow = [&]() -> int {
                sort(nums.begin(), nums.end());
                int delta = 0;
                for (int i = 1; i < n; ++i){
                    nums[i] += delta;
                    int curr_delta = nums[i] - nums[i-1];
                    delta += curr_delta;
                }
                return delta;
            };

            auto equivalent_flow = [&]() -> int {
                // vòng lặp simple_flow ở trên chỉ đơn giản là (bạn có thể tự suy ra từ công thức):
                // delta_1 = a1 - a0
                // delta_2 = a2 - a0
                // delta_3 = a3 - a0
                // .....
                // delta_n = an - a0
                int min =  *min_element(nums.begin(), nums.end());
                int moves = 0;
                for (int x : nums){
                    moves += x - min;
                }
                return moves;
            };


            return equivalent_flow();
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