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
    LC 2089 - Find Target Indices After Sorting Array
    https://leetcode.com/problems/find-target-indices-after-sorting-array/

    You are given a 0-indexed integer array nums and a target element target.

    A target index is an index i such that nums[i] == target.

    Return a list of the target indices of nums after sorting nums in non-decreasing order. If there are no target indices, return an empty list. The returned list must be sorted in increasing order.

    Example 1:

    Input: nums = [1,2,5,2,3], target = 2
    Output: [1,2]
    Explanation: After sorting, nums is [1,2,2,3,5].
    The indices where nums[i] == 2 are 1 and 2.

    Example 2:

    Input: nums = [1,2,5,2,3], target = 3
    Output: [3]
    Explanation: After sorting, nums is [1,2,2,3,5].
    The index where nums[i] == 3 is 3.

    Example 3:

    Input: nums = [1,2,5,2,3], target = 5
    Output: [4]
    Explanation: After sorting, nums is [1,2,2,3,5].
    The index where nums[i] == 5 is 4.

    Constraints:

    - 1 <= nums.length <= 100
    - 1 <= nums[i], target <= 100
*/
/*
    Best solution: counting — no sort needed

    Complexity:
        Time:  O(n) — one pass to count.
        Space: O(1) extra besides the output.

    Approach (detail):
        1. After sorting, every element smaller than target sits before every copy of target, so the
           first index of target is exactly smaller = #(nums[i] < target).
        2. The copies of target occupy the next equal = #(nums[i] == target) positions.
        3. Return [smaller, smaller + 1, ..., smaller + equal - 1] (empty when equal = 0).
*/
/*
    Your solution: count smaller and equal, emit the consecutive indices

    Complexity:
        Time:  O(n) — one counting loop plus the output.
        Space: O(1) extra besides ans.

    Approach:
        1. One loop counts smaller (values < target) and equal (values == target).
        2. ans gets smaller + i for i = 0 .. equal - 1.

    Verdict: MATCHES BEST — the O(n) counting solution, no sort. Verified against sort-and-scan on
        20000 random arrays.
*/
class Solution {
    public:
        vector<int> targetIndices(vector<int>& nums, int target) {
            int smaller = 0;
            int equal = 0;
            for (int i = 0; i < (int)nums.size(); ++i){
                smaller += (nums[i] < target);
                equal += (nums[i] == target);
            }
            vector<int> ans;
            for (int i = 0; i < equal; ++i){
                ans.push_back(smaller + i);
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