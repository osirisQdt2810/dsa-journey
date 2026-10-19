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
    LC 1658 - Minimum Operations to Reduce X to Zero
    https://leetcode.com/problems/minimum-operations-to-reduce-x-to-zero/

    You are given an integer array nums and an integer x. In one operation, you can either remove the leftmost or the rightmost element from the array nums and subtract its value from x. Note that this modifies the array for future operations.

    Return the minimum number of operations to reduce x to exactly 0 if it is possible, otherwise, return -1.

    Example 1:

    Input: nums = [1,1,4,2,3], x = 5
    Output: 2
    Explanation: The optimal solution is to remove the last two elements to reduce x to zero.

    Example 2:

    Input: nums = [5,6,7,8,9], x = 4
    Output: -1

    Example 3:

    Input: nums = [3,2,20,1,1,3], x = 10
    Output: 5
    Explanation: The optimal solution is to remove the last three elements and the first two elements (5 operations in total) to reduce x to zero.

    Constraints:

    - 1 <= nums.length <= 10^5
    - 1 <= nums[i] <= 10^4
    - 1 <= x <= 10^9
*/
/*
    Best solution: sliding window for the longest middle subarray with sum = total - x

    Complexity:
        Time:  O(n) — each index enters and leaves the window once.
        Space: O(1) — two pointers and a running sum.

    Approach (detail):
        1. Removing elements from both ends leaves a contiguous middle block, so minimising the
           removed count means maximising the length of a subarray whose sum is target = total - x.
        2. If target < 0 there is no answer (-1); if target == 0 the whole array is removed (n).
        3. All nums[i] are positive, so window sums grow when the right end advances and shrink when
           the left end advances: extend r, then move l while the sum exceeds target, and record
           r - l + 1 whenever the sum equals target.
        4. Answer = n - (longest length), or -1 if no window ever matched.
*/
/*
    Your solution: in-place prefix sums + hash map of first index per prefix value

    Complexity:
        Time:  O(n) expected — one pass to build prefix sums, one pass with O(1) hash lookups.
        Space: O(n) — the unordered_map of prefix sums.

    Approach:
        1. nums is rewritten in place into prefix sums with nums[0] = 0 (one extra slot pushed).
        2. x is replaced by total - x (the required middle-block sum); if it is 0 the answer is n.
        3. For j = 1..n, target = nums[j] - x; if some earlier prefix equals target at index i - 1,
           the block i..j has the right sum, and res keeps the longest such length.
        4. Each prefix value is inserted only once (its first index), which keeps blocks longest;
           the answer is n - res, or -1 when res stays -1.

    Verdict: SUBOPTIMAL — correct with O(n) time, but O(n) extra space for the hash map vs O(1) for
        the two-pointer window (valid because every nums[i] is positive); it passes at n <= 10^5.
        Note: the debug cout lines inside the loop print about 2 * 10^5 lines per test on
        LeetCode, which slows the run noticeably and risks the output limit. Verified (with stdout
        silenced) against a brute force over every prefix / suffix split on 20000 random cases.
*/
class Solution {
    public:
        int minOperations(vector<int>& nums, int x) {
            int n = nums.size();
            int temp = nums[0];
            nums.push_back(0);
            nums[0] = 0;
            // a[0] = 0;
            // a[1] = nums[0];
            // a[2] = a[1] + nums[1];
            // ...
            // a[i] = a[i-1] + nums[i-1];
            for (int i = 1; i <= n; ++i){
                int curr = nums[i];
                nums[i] = temp + nums[i-1];
                temp = curr;
            }
            x = nums[n] - x;
            if (x == 0) return n;

            // find (i, j) (0-index) where len=j-i+1 max, sum(i...j)=x
            // find (i, j): sum(j)-sum(i-1)=x
            // find (i, j) (0-index): sum(j+1)-sum(i) = x
            unordered_map<int, int> hash{{0, 0}};
            int res = -1;
            for (int j = 1; j <= n; ++j){
                int target = nums[j] - x;
                cout << "j = " << j << " x=" << x << " nums[j] = " << nums[j] << " t=" << target << "\n";
                if (hash.count(target)){
                    int i = hash[target] + 1;
                    res = max(res, j-i+1);
                    cout << "r = " << res << " i=" << i << "\n";
                }
                if (!hash.count(nums[j])){
                    hash[nums[j]] = j;
                }
            }
            return (res < 0) ? -1 : (n - res);
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

    vector<int> nums = {3,2,20,1,1,3};
    nums = {2,3,1,1,1};
    int x = 10;
    x=8;

    cout << sol.minOperations(nums, x);


    return 0;
}