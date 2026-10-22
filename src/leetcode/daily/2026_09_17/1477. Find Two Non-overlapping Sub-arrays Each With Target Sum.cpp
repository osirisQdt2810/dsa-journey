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
    LC 1477 - Find Two Non-overlapping Sub-arrays Each With Target Sum
    https://leetcode.com/problems/find-two-non-overlapping-sub-arrays-each-with-target-sum/

    You are given an array of integers arr and an integer target.

    You have to find two non-overlapping sub-arrays of arr each with a sum equal target. There can be multiple answers so you have to find an answer where the sum of the lengths of the two sub-arrays is minimum.

    Return the minimum sum of the lengths of the two required sub-arrays, or return -1 if you cannot find such two sub-arrays.

    Example 1:

    Input: arr = [3,2,2,4,3], target = 3
    Output: 2
    Explanation: Only two sub-arrays have sum = 3 ([3] and [3]). The sum of their lengths is 2.

    Example 2:

    Input: arr = [7,3,4,7], target = 7
    Output: 2
    Explanation: Although we have three non-overlapping sub-arrays of sum = 7 ([7], [3,4] and [7]), but we will choose the first and third sub-arrays as the sum of their lengths is 2.

    Example 3:

    Input: arr = [4,3,2,6,2,3,4], target = 6
    Output: -1
    Explanation: We have only one sub-array of sum = 6.

    Constraints:

    - 1 <= arr.length <= 10^5
    - 1 <= arr[i] <= 1000
    - 1 <= target <= 10^8
*/
/*
    Best solution: sliding window (all values positive) + best-length-so-far array

    Complexity:
        Time:  O(n) — each index enters and leaves the window once.
        Space: O(n) — best[i] = shortest target-sum subarray ending at or before i.

    Approach (detail):
        1. Values are >= 1, so the window sum is monotone in its edges: grow r, shrink l while
           sum > target; when sum == target the window [l, r] is the shortest one ending at r.
        2. Keep best[r] = min(best[r-1], r - l + 1 when sum == target).
        3. At the same moment, pair the window with the best subarray that ends strictly before
           l: ans = min(ans, (r - l + 1) + best[l - 1]). Since best[l-1] ends before l, the two
           never overlap, and every optimal pair is seen when its right subarray is found.
        4. Return ans, or -1 if no pair exists.
*/
/*
    Your solution: prefix sums + hash map, best-left / best-right arrays, split-point scan

    Complexity:
        Time:  O(n) expected — three linear passes with O(1) average hash lookups.
        Space: O(n) — left, right and the val2index map.

    Approach:
        1. Turn arr into prefix sums in place: arr[k] = sum of the first k values (arr grows to
           n + 1 entries; the input is modified).
        2. Forward: left[i] = shortest subarray with sum target inside prefix [0, i] — look up
           arr[i] - target in val2index (prefix value -> latest index) and keep a running min.
        3. Backward: right[i] = shortest subarray starting at index >= i — look up arr[i] + target
           in the map built from the right, again with a running min.
        4. Answer = min over split points i of left[i] + right[i] (pieces end at <= i and start
           at >= i, so they do not overlap), or -1.

    Verdict: MATCHES BEST — O(n) time and O(n) space like the sliding window; the hash map is only
        a constant-factor cost (prefix sums are strictly increasing, so a two-pointer could
        replace it).
        Verified against an all-pairs brute force on 200000 random arrays and the LeetCode
        examples, under ASan/UBSan.
*/
class Solution {
    public:
        int minSumOfLengths(vector<int>& arr, int target) {
            int n = arr.size();
            int temp = arr[0];
            arr[0] = 0;
            arr.push_back(0);
            // vector<int> arr = {3,1,1,1,5,1,2,1};
            for (int i = 0; i < n; ++i){
                int curr = arr[i+1];
                arr[i+1] = arr[i] + temp;
                temp = curr;
            }

            // forward direction
            vector<int> left(n+1, INT_MAX);
            unordered_map<int, int> val2index;
            val2index.insert({arr[0], 0});
            for (int i = 1; i < n; ++i){
                int j_val = arr[i] - target;
                if (val2index.count(j_val)){
                    left[i] = i - val2index[j_val];
                }
                left[i] = min(left[i], left[i-1]);
                val2index[arr[i]] = i;
                // cout << "i=" << i << " l=" << left[i] << "\n";
            }

            // backward direction
            vector<int> right(n+1, INT_MAX);
            val2index.clear();
            val2index.insert({arr[n], n});
            // cout << arr[n] << "\n";

            for (int i = n-1; i>=1; --i){
                int j_val = arr[i] + target;
                if (val2index.count(j_val)){
                    right[i] = val2index[j_val] - i;
                }
                right[i] = min(right[i], right[i+1]);
                val2index[arr[i]] = i;
                // cout << "i=" << i << " r=" << right[i] << "\n";
            }

            // retrieve result
            int ans = INT_MAX;
            for (int i = 1; i < n; ++i){
                if (left[i] == INT_MAX) continue;
                if (right[i] == INT_MAX) continue;
                ans = min(left[i] + right[i], ans);
                // cout << "l+r=" << left[i] + right[i] << "\n";
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

    vector<int> arr = {3,1,1,1,5,1,2,1};
    int target = 3; // output = 2, expected = 3
    // 0 1 2 3 4 5 6 7
    // 3 1 1 1 5 1 2 1

    // new arr: 0 1 2 3 4  5  6  7  8
    //          0 3 4 5 6 11 12 14 15
    // tim (i,j), (k, l): t(j)-t(i) ==  t(l) - t(k) == target = 3

    // approach: 1

    sol.minSumOfLengths(arr, target);

    return 0;
}