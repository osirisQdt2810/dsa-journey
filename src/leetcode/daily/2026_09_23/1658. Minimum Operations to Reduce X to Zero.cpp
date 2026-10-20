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
    Your solution: two-pointer sliding window for the longest block with sum = total - x (sliding_window)

    Complexity:
        Time:  O(n) — one pass for the total, then each index enters and leaves the window once.
        Space: O(1) — l, r and the running sum.

    Approach:
        1. x is replaced by total - x, the sum the kept middle block must have.
        2. For each r: sum += nums[r]; while l <= r and sum > x, drop nums[l] and advance l.
        3. When sum == x, res = max(res, r - l + 1). The l <= r guard lets the window shrink to empty,
           which handles a negative target (never matches, -1) and a zero target (empty block, n).
        4. Return n - res, or -1 if no block matched.
        Also in the file (not called): suboptimal — prefix sums + hash map of first index per prefix,
        O(n) time but O(n) space.

    Verdict: MATCHES BEST — the O(n) / O(1) sliding window. Verified against a brute force over every
        prefix / suffix split on 20000 random cases, the problem's examples, and n = 10^5 edge cases
        (target 0 and target 10^4).
*/
class Solution {
    public:
        int minOperations(vector<int>& nums, int x) {
            int n = nums.size();

            auto suboptimal = [&]() -> int {
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
                    // cout << "j = " << j << " x=" << x << " nums[j] = " << nums[j] << " t=" << target << "\n";
                    if (hash.count(target)){
                        int i = hash[target] + 1;
                        res = max(res, j-i+1);
                        // cout << "r = " << res << " i=" << i << "\n";
                    }
                    if (!hash.count(nums[j])){
                        hash[nums[j]] = j;
                    }
                }
                return (res < 0) ? -1 : (n - res);
            };

            auto sliding_window = [&]() -> int {
                int l = 0;
                int sum = 0;
                int res = -1;
                for (int r = 0; r < n; ++r) sum += nums[r];
                x = sum - x;
                sum = 0;
                for (int r = 0; r < n; ++r){
                    sum += nums[r];
                    while (l <= r && sum > x){
                        sum -= nums[l];
                        ++l;
                    }
                    if (sum == x){
                        res = max(res, r - l + 1);
                    }
                }
                return (res < 0) ? -1 : (n - res);
            };

            return sliding_window();
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