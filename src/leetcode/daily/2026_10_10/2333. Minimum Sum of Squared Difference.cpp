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
    LC 2333 - Minimum Sum of Squared Difference
    https://leetcode.com/problems/minimum-sum-of-squared-difference/

    Problem:
        You are given two positive 0-indexed integer arrays nums1 and nums2, both of length n. The sum
        of squared difference is the sum of (nums1[i] - nums2[i])^2 for each 0 <= i < n. You are also
        given two positive integers k1 and k2: you can modify any element of nums1 by +1 or -1 at most
        k1 times, and any element of nums2 by +1 or -1 at most k2 times. Return the minimum sum of
        squared difference after these modifications. Elements may become negative.

        Constraints:
            n == nums1.length == nums2.length
            1 <= n <= 10^5
            0 <= nums1[i], nums2[i] <= 10^5
            0 <= k1, k2 <= 10^9

    Examples:
        Input: nums1 = [1,2,3,4], nums2 = [2,10,20,19], k1 = 0, k2 = 0
        Output: 579
        Explanation: nothing can be modified: 1 + 64 + 289 + 225 = 579.

        Input: nums1 = [1,4,10,12], nums2 = [5,8,6,9], k1 = 1, k2 = 1
        Output: 43
        Explanation: increase nums1[0] once and nums2[2] once: 9 + 16 + 9 + 9 = 43.

    Best solution: greedy "water-level" flattening of the largest |nums1[i] - nums2[i]|, with
    k = k1 + k2 as one shared budget (sort + sweep, or binary search on the level / counting buckets).

    Complexity:
        Time:  O(n log n) for the sort (O(n + max|d|) with counting buckets instead).
        Space: O(1) extra beyond the in-place array of differences (O(max|d|) with buckets).

    Approach (detail):
        1. Only d[i] = |nums1[i] - nums2[i]| matters, and a +-1 on either array changes one d[i] by 1,
           so k1 and k2 merge into one budget k = k1 + k2 (<= 2e9, still fits in int).
        2. If sum(d) <= k, every difference can be driven to 0 -> answer 0.
        3. Otherwise, since x^2 is convex, each unit of budget should hit a current maximum. Sort d
           descending (with a 0 sentinel) and lower the top `anchor` elements together to the next
           value d[anchor]; this costs anchor * (d[anchor-1] - d[anchor]).
        4. When the next step is unaffordable, lower the top block by m = k / anchor more, then spend
           the remaining r = k % anchor units to lower r of them by one more: the block contributes
           r * (v-1)^2 + (anchor - r) * v^2, and the untouched tail adds sum d[i]^2.
        Correctness: by exchange, moving a unit from a larger to a smaller difference never lowers a
        sum of squares, so the optimum makes the largest values as equal as possible.

    Your solution:
        MATCHES BEST — sort descending + water-level sweep exactly as above, O(n log n) time and O(1)
        extra space (it reuses nums1 to hold the differences). Verified against a unit-by-unit
        max-heap brute force on random tests and on the k1 = k2 = 1e9 extreme.
*/

/*
    Example 1:

    Input: nums1 = [1,2,3,4], nums2 = [2,10,20,19], k1 = 0, k2 = 0
    Output: 579
    Explanation: The elements in nums1 and nums2 cannot be modified because k1 = 0 and k2 = 0.
    The sum of square difference will be: (1 - 2)2 + (2 - 10)2 + (3 - 20)2 + (4 - 19)2 = 579.
*/
class Solution {
    public:
        long long minSumSquareDiff(vector<int>& nums1, vector<int>& nums2, int k1, int k2) {
            // min(sum(a[i]-b[i])^2)
            // 1 3 4 5 6 7 9
            int bound = k1 + k2;
            int n = nums1.size();
            for (int i = 0; i < n; ++i){
                nums1[i] = abs(nums1[i] - nums2[i]);
            }

            sort(nums1.begin(), nums1.end(), std::greater<int>());
            nums1.push_back(0);
            long long sum = 0;    // nums2[m] = a[1]+...+a[m]
            for (int i = 0; i < n; ++i){
                sum += nums1[i];
            }
            if (sum <= bound) return 0;

            // for (int i = 0; i < n; ++i) cout << nums1[i] << " ";
            // cout << "\n";
            // for (int i = 0; i < n; ++i) cout << nums2[i] << " ";
            // cout << "\n";

            // find anchor K, where: a1...aK-1 -> aK,
            // diff: a1-aK, a2-aK...aK-1-aK
            // sum diff: (a1+...+aK-1) - (k-1).aK <= bound
            auto sum_square = [&]() -> long long {
                long long res = 0;
                for (int i = 0; i < n; ++i){
                    res += 1LL * nums1[i] * nums1[i];
                }
                return res;
            };

            int anchor = 1;
            while (anchor <= n){
                long long k = 1LL * anchor * (nums1[anchor-1] - nums1[anchor]);
                if (k <= bound){
                    bound -= k;
                    ++anchor;
                }
                else {
                    int v = nums1[anchor-1];
                    int m = bound / anchor;
                    bound -= m * anchor;
                    v -= m;
                    long long ans = 1LL * bound * (v-1)*(v-1) + 1LL * (anchor-bound) * v * v;
                    for (int i = anchor; i < n; ++i){
                        ans += 1LL * nums1[i] * nums1[i];
                    }
                    return ans;
                }
            }

            return sum_square();
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
    vector<int> nums1 = {1,4,10,12}, nums2 = {5,8,6,9}; int k1 = 1, k2 = 1;
    cout << sol.minSumSquareDiff(nums1, nums2, k1, k2) << "\n";

    return 0;
}