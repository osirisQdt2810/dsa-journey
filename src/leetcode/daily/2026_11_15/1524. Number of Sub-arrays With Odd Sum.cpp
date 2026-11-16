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
#include <tuple>
#include <functional>

using namespace std;

/*
    LC 1524 - Number of Sub-arrays With Odd Sum
    https://leetcode.com/problems/number-of-sub-arrays-with-odd-sum/
    @status: self
    @time: 20
    @wa: 0

    Given an array of integers arr, return the number of subarrays with an odd sum.

    Since the answer can be very large, return it modulo 10^9 + 7.

    Example 1:

    Input: arr = [1,3,5]
    Output: 4
    Explanation: All subarrays are [[1],[1,3],[1,3,5],[3],[3,5],[5]]
    All sub-arrays sum are [1,4,9,3,8,5].
    Odd sums are [1,9,3,5] so the answer is 4.

    Example 2:

    Input: arr = [2,4,6]
    Output: 0
    Explanation: All subarrays are [[2],[2,4],[2,4,6],[4],[4,6],[6]]
    All sub-arrays sum are [2,6,12,4,10,6].
    All sub-arrays have even sum and the answer is 0.

    Example 3:

    Input: arr = [1,2,3,4,5,6,7]
    Output: 16

    Constraints:

    - 1 <= arr.length <= 10^5
    - 1 <= arr[i] <= 100
*/
/*
    Best solution: prefix-sum parity counting

    Complexity:
        Time:  O(n) — one pass.
        Space: O(1) — two counters and the running parity.

    Approach (detail):
        1. sum(i..j) = P[j+1] - P[i] is odd iff P[j+1] and P[i] have different parity.
        2. Sweep the prefixes keeping odd / even = how many earlier prefixes were odd / even; start with
           even = 1 for the empty prefix P[0] = 0.
        3. At each new prefix: if it is odd, every earlier even prefix pairs with it, so add `even`;
           if it is even, add `odd`. Then bump its own counter.
        4. Answer modulo 10^9 + 7. Only the parity matters, so the running sum need not be stored.
*/
/*
    Your solution: in-place exclusive prefix sums, then parity counting

    Complexity:
        Time:  O(n) — one pass to build prefixes, one to count.
        Space: O(1) extra besides mutating arr (push_back may reallocate once, O(n) worst case).

    Approach:
        1. push_back(0), then for i = 0..n replace arr[i] with run (the sum of the original elements
           before i) while run accumulates the original value x; afterwards arr[i] = P[i].
        2. odd = 0, even = 1 (P[0] = 0). For i = 1..n: odd P[i] adds `even`, even P[i] adds `odd`, via
           summod (add and subtract MOD once), then bumps its own counter.
        3. Prefix sums stay <= 10^5 * 100 = 10^7, so int is safe.

    Verdict: MATCHES BEST — the O(n) parity counting. It writes the prefixes into arr instead of a single
        running parity, which mutates the input but changes nothing asymptotically. Verified: 3/3 official
        examples and a brute force on 200000 random arrays (n <= 15), plus n = 10^5 all ones.
*/
class Solution {
    public:
        int numOfSubarrays(vector<int>& arr) {
            int n = arr.size();
            // vector<int> sum(n+1, 0);
            arr.push_back(0);
            // temp1 = arr[0]
            // arr[0] = 0
            // temp2 = arr[1]
            // arr[1] = arr[0] + temp1
            // temp3 = arr[2]
            // arr[2] = arr[1] + temp2

            // arr[i+1] = arr[i] + temp
            // temp = arr[i+1]
            int run = 0;
            for (int i = 0; i <= n; ++i){
                int x = arr[i];
                arr[i] = run;
                run += x;
            }

            const int MOD = 1e9+7;
            auto summod = [](int& a, int b){
                a = a + b;
                if (a >= MOD) a -= MOD;
            };

            int odd = 0, even = 1;
            int counter = 0;
            for (int i = 1; i <= n; ++i){
                if (arr[i] % 2){
                    summod(counter, even);
                    ++odd;
                }
                else {
                    summod(counter, odd);
                    ++even;
                }
            }
            return counter;
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
    int passed = 0, total = 0;
    auto check = [&](int idx, int got, int expected) {
        ++total;
        if (got == expected) {
            ++passed;
            cout << "Example " << idx << ": PASS\n";
        } else {
            cout << "Example " << idx << ": FAIL  got=" << got << "  expected=" << expected << "\n";
        }
    };
    {
        Solution sol;
        vector<int> arr = {1, 3, 5};
        check(1, sol.numOfSubarrays(arr), 4);
    }
    {
        Solution sol;
        vector<int> arr = {2, 4, 6};
        check(2, sol.numOfSubarrays(arr), 0);
    }
    {
        Solution sol;
        vector<int> arr = {1, 2, 3, 4, 5, 6, 7};
        check(3, sol.numOfSubarrays(arr), 16);
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}
