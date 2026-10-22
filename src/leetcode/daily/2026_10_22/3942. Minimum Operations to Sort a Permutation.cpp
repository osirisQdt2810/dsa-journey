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
    LC 3942 - Minimum Operations to Sort a Permutation
    https://leetcode.com/problems/minimum-operations-to-sort-a-permutation/

    You are given an integer array nums of length n, where nums is a permutation of the integers from 0 to n - 1.

    You may perform only the following operations:

    - Reverse the entire array.
    - Rotate Left by One: Move the first element to the end of the array, and rest elements to left by one position.

    Return an integer denoting the minimum number of operations required to sort the array in increasing order. If it is not possible to sort the array using only the given operations, return -1.

    Example 1:

    Input: nums = [0,2,1]
    Output: 2
    Explanation:
    - Rotate Left by one: [2, 1, 0]
    - Reverse the array: [0, 1, 2]
    The array becomes sorted in 2 operations, which is minimal

    Example 2:

    Input: nums = [1,0,2]
    Output: 2
    Explanation:
    - Reverse the array: [2, 0, 1]
    - Rotate Left by one: [0, 1, 2]
    The array becomes sorted in 2 operations, which is minimal.

    Example 3:

    Input: nums = [2,0,1,3]
    Output: -1
    Explanation:
    It is impossible to reach [2, 0, 1, 3]. Thus, the answer is -1.

    Constraints:

    - 1 <= n == nums.length <= 10^5
    - 0 <= nums[i] <= n - 1
    - nums is a permutation of integers from 0 to n - 1.
*/
/*
    Best solution: recognise a rotation of the sorted (or reversed) array, then take the cheaper
    of "rotate only" and "reverse + rotate" on each side

    Complexity:
        Time:  O(n) — one scan for descents, one for ascents.
        Space: O(1).

    Approach (detail):
        1. Both operations preserve cyclic adjacency (rotation) or reverse it (reversal), so nums
           is sortable iff it is a rotation of 0..n-1 or a rotation of n-1..0; anything else -> -1.
        2. Ascending rotation: exactly one descent at index p (nums[p] > nums[p+1]) and
           nums[n-1] < nums[0]. Either rotate left p+1 times, or reverse, rotate left n-p-1
           times and reverse again: n-p+1 operations. (No descent at all -> already sorted, 0.)
        3. Descending rotation: exactly one ascent at p and nums[n-1] > nums[0]. Either rotate left
           p+1 times then reverse (p+2), or reverse first then rotate left n-p-1 times (n-p).
           A fully descending array needs just one reverse.
        4. Using reverse more than twice never helps (two reversals cancel), so the minimum over
           these candidates is optimal.
*/
/*
    Your solution: one-descent / one-ascent scan with four candidate costs

    Complexity:
        Time:  O(n) — two linear scans.
        Space: O(1).

    Approach:
        1. Forward scan: pivot = index of the only descent (-2 if there are several, -1 if none);
           none -> return 0. With one descent and nums[n-1] <= nums[0], take
           min(pivot + 1, n - pivot + 1).
        2. Backward scan: pivot = index of the only ascent; none -> return 1 (one reverse).
           With one ascent and nums[n-1] >= nums[0], take min(2 + pivot, n - pivot).
        3. Return the best candidate, or -1 if neither pattern matched.

    Verdict: MATCHES BEST — the O(n) / O(1) rotation-pattern check with the four candidate costs.
        Verified against a BFS over both operations for every permutation with n <= 8 (46233).
*/
class Solution {
    public:
        int minOperations(vector<int>& nums) {
            int n = nums.size();
            if (n == 1) return 0;

            int ans = INT_MAX;
            // forward way
            int pivot = -1;
            for (int i = 0; i < n-1; ++i){
                if (nums[i] > nums[i+1]){
                    if (pivot < 0){
                        pivot = i;
                        continue;
                    }
                    pivot = -2;
                    break;
                }
            }
            if (pivot == -1) return 0;
            if (pivot >= 0 && nums[n-1] <= nums[0]){
                ans = min(ans, pivot+1);
                ans = min(ans, n-pivot+1);
            }

            // backward way
            pivot = -1;
            for (int i = 0; i < n-1; ++i){
                if (nums[i] < nums[i+1]){
                    if (pivot < 0){
                        pivot = i;
                        continue;
                    }
                    pivot = -2;
                    break;
                }
            }
            if (pivot == -1) return 1;
            if (pivot >= 0 && nums[n-1] >= nums[0]){
                ans = min(ans, 2 + pivot);
                ans = min(ans, n-pivot);
            }

            return (ans == INT_MAX) ? -1 : ans;
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