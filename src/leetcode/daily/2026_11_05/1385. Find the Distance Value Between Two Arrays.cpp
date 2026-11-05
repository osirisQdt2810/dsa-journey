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
    LC 1385 - Find the Distance Value Between Two Arrays
    https://leetcode.com/problems/find-the-distance-value-between-two-arrays/

    Given two integer arrays arr1 and arr2, and the integer d, return the distance value between the two arrays.

    The distance value is defined as the number of elements arr1[i] such that there is not any element arr2[j] where |arr1[i]-arr2[j]| <= d.

    Example 1:

    Input: arr1 = [4,5,8], arr2 = [10,9,1,8], d = 2
    Output: 2
    Explanation:
    For arr1[0]=4 we have:
    |4-10|=6 > d=2
    |4-9|=5 > d=2
    |4-1|=3 > d=2
    |4-8|=4 > d=2
    For arr1[1]=5 we have:
    |5-10|=5 > d=2
    |5-9|=4 > d=2
    |5-1|=4 > d=2
    |5-8|=3 > d=2
    For arr1[2]=8 we have:
    |8-10|=2 <= d=2
    |8-9|=1 <= d=2
    |8-1|=7 > d=2
    |8-8|=0 <= d=2

    Example 2:

    Input: arr1 = [1,4,2,3], arr2 = [-4,-3,6,10,20,30], d = 3
    Output: 2

    Example 3:

    Input: arr1 = [2,1,100,3], arr2 = [-5,-2,10,-3,7], d = 6
    Output: 1

    Constraints:

    - 1 <= arr1.length, arr2.length <= 500
    - -1000 <= arr1[i], arr2[j] <= 1000
    - 0 <= d <= 100
*/
/*
    Best solution: sort arr2 + binary search for the nearest element

    Complexity:
        Time:  O((n + m) log m) — sort arr2 once, then one lower_bound per arr1 element.
        Space: O(1) extra beyond the in-place sort (O(log m) sort stack).

    Approach (detail):
        1. x is counted iff no arr2 value lies in the window [x - d, x + d].
        2. Sort arr2. For each x, it = lower_bound(arr2, x - d) is the smallest value >= x - d.
        3. The window is empty iff it == end or *it > x + d — one check, no scan of arr2.
        4. (Alternative: sort both arrays and sweep with two pointers, O(n log n + m log m).)
*/
/*
    Your solution: brute force over all (num1, num2) pairs

    Complexity:
        Time:  O(n * m) — for each num1, scan arr2 until a close element is found.
        Space: O(1) — only the ok flag and the counter.

    Approach:
        1. For each num1 in arr1, set ok = false.
        2. Scan arr2; if abs(num1 - num2) <= d set ok = true and break early.
        3. ans += !ok — count num1 only when no num2 is within distance d. Return ans.

    Also in the file (commented out): a min2/max2 bound of arr2 — only a filter, not a full answer.

    Verdict: SUBOPTIMAL — O(n * m) vs O((n + m) log m); at n, m <= 500 that is 250000 checks,
        so it passes easily. A direct transcription of the definition, correct by construction.
*/
class Solution {
    public:
        int findTheDistanceValue(vector<int>& arr1, vector<int>& arr2, int d) {
            // int min2 = arr2[0], max2 = arr2[0], n2 = arr2.size();
            // for (int i = 1; i < n2; ++i){
            //     min2 = min(min2, arr2[i]);
            //     max2 = max(max2, arr2[i]);
            // }
            // max2 += d;
            // min2 -= d;
            // cout << max2 << " " << min2 << "\n";
            int ans = 0;
            for (auto& num1 : arr1){
                bool ok = false;
                for (auto& num2 : arr2){
                    if (abs(num1 - num2) <= d){
                        ok = true;
                        break;
                    }
                }
                ans += (!ok);
            }
            // mọi a2: |X - a2| > d => X > a2 + d hoặc X < a2 - d
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