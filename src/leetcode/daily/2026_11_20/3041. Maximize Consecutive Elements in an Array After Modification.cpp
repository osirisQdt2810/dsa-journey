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
#include <cmath>
#include <tuple>
#include <functional>

using namespace std;

/*
    LC 3041 - Maximize Consecutive Elements in an Array After Modification
    https://leetcode.com/problems/maximize-consecutive-elements-in-an-array-after-modification/
    @status: hint
    @time: 23
    @wa: 1
    @created: 2026-11-20 09:02

    You are given a 0-indexed array nums consisting of positive integers.

    Initially, you can increase the value of any element in the array by at most 1.

    After that, you need to select one or more elements from the final array such that those elements are consecutive when sorted in increasing order. For example, the elements [3, 4, 5] are consecutive while [3, 4, 6] and [1, 1, 2, 3] are not.

    Return the maximum number of elements that you can select.

    Example 1:

    Input: nums = [2,1,5,1,1]
    Output: 3
    Explanation: We can increase the elements at indices 0 and 3. The resulting array is nums = [3,1,5,2,1].
    We select the elements [3,1,5,2,1] and we sort them to obtain [1,2,3], which are consecutive.
    It can be shown that we cannot select more than 3 consecutive elements.

    Example 2:

    Input: nums = [1,4,7,10]
    Output: 1
    Explanation: The maximum consecutive elements that we can select is 1.

    Constraints:

    - 1 <= nums.length <= 10^5
    - 1 <= nums[i] <= 10^6
*/
/*
    Best solution: sort + DP indexed by final value

    Complexity:
        Time:  O(n log n) — the sort; the DP is one pass.
        Space: O(V) — dp array over values, V = max(nums) + 2 (O(n) with a hash map).

    Approach (detail):
        1. Each element ends as v or v+1; the chosen elements must form distinct consecutive values.
        2. dp[x] = longest run of consecutive values ending exactly at value x.
        3. Process nums in ascending order. For an element v, it can become v+1 (extend a run ending at v)
           or v (extend a run ending at v-1).
        4. Update dp[v+1] = dp[v] + 1 FIRST, then dp[v] = dp[v-1] + 1, so one element is never used twice
           (using old dp[v], not the one just updated from v itself).
        5. Duplicates are handled automatically: the second copy of v can take value v+1 after the first took v.
        6. The answer is the maximum dp value seen.
*/
/*
    Your solution: sort + dp[value] updated as dp[v+1] first, then dp[v]

    Complexity:
        Time:  O(n log n) — sort plus one pass.
        Space: O(V) — vector<int> of size 10^6 + 2.

    Approach:
        1. sort(nums); dp(1e6 + 2, 0); ans = 1.
        2. For each v = nums[i]: dp[v+1] = dp[v] + 1, then dp[v] = dp[v-1] + 1.
        3. ans = max(ans, dp[v], dp[v+1]).
        4. return ans.

    Verdict: MATCHES BEST — the documented sort + value-DP.
        Verified against a brute force (every increment mask, every subset) on 20000 random arrays, n <= 8.
        (An earlier index-based DP over a[i-1] was INCORRECT: nums = {2,2,1,2,3} gave 3, correct is 4.)
*/
class Solution {
    public:
        int maxSelectedElements(vector<int>& nums) {
        // a1 a2 a3 a4 a5
        // 1 1 1 2 5

        // if (a[i] - a[i-1] > 2) dp(i)(0) = dp(i)(1) = 1
        // if (a[i] - a[i-1] = 1)
        // dp(i)(0) = dp(i-1)(0) + 1
        // dp(i)(1) = 1
        // if (a[i] - a[i-1] = 0)
        // dp(i)(0) = 1
        // dp(i)(1) = dp(i-1)(0) + 1
        // if (a[i] - a[i-1] = 2)
        // dp(i)(0) = dp(i-1)(1) + 1
        // dp(i)(1) = 1

        // 1 2 2 2 3
        sort(nums.begin(), nums.end());
        int n = nums.size();
        int ans = 1;
        vector<int> dp(1e6+2, 0);
        for (int i = 0; i < n; ++i){
            int v = nums[i];
            dp[v+1] = dp[v]+1;
            dp[v] = dp[v-1]+1;
            ans = max(ans, dp[v]);
            ans = max(ans, dp[v+1]);
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
    // test-lc:begin
    // test-lc:end
    return 0;
}
