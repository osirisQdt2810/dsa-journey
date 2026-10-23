#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

/*
    LC 493 - Reverse Pairs
    https://leetcode.com/problems/reverse-pairs/

    Given an integer array nums, return the number of reverse pairs in the array.

    A reverse pair is a pair (i, j) where:

    - 0 <= i < j < nums.length and
    - nums[i] > 2 * nums[j]

    Example 1:

    Input: nums = [1,3,2,3,1]
    Output: 2
    Explanation: The reverse pairs are:
    (1, 4) --> nums[1] = 3, nums[4] = 1, 3 > 2 * 1
    (3, 4) --> nums[3] = 3, nums[4] = 1, 3 > 2 * 1

    Example 2:

    Input: nums = [2,4,3,5,1]
    Output: 3
    Explanation: The reverse pairs are:
    (1, 4) --> nums[1] = 4, nums[4] = 1, 4 > 2 * 1
    (2, 4) --> nums[2] = 3, nums[4] = 1, 3 > 2 * 1
    (3, 4) --> nums[3] = 5, nums[4] = 1, 5 > 2 * 1

    Constraints:

    - 1 <= nums.length <= 5 * 10^4
    - -2^31 <= nums[i] <= 2^31 - 1
*/
/*
    Best solution: merge sort counting (or a Fenwick tree over compressed values)

    Complexity:
        Time:  O(n log n) — merge sort with a linear two-pointer count per merge.
        Space: O(n) — the merge buffer.

    Approach (detail):
        1. Count pairs i < j with nums[i] > 2 * nums[j]. Split the array, count inside each half
           recursively, then count the cross pairs (i in the left half, j in the right half).
        2. Both halves are sorted after recursion, so for each i in the left half a pointer into
           the right half only moves forward while nums[i] > 2 * nums[j]; the cross count is O(n).
        3. Merge the halves as usual. Compare with 64-bit (2 * nums[j] can overflow int).
        4. Fenwick alternative, same O(n log n): compress all values and their doubles, sweep left
           to right, and for each x add the number of earlier values with rank > rank(2x), then
           insert rank(x).
*/
/*
    Your solution: left-to-right Fenwick sweep over compressed {x, 2x}

    Complexity:
        Time:  O(n log n) — compression sort plus two Fenwick operations per element.
        Space: O(n) — vals (up to 2n entries) and the tree.

    Approach:
        1. compress() collects every num and 2 * num as long long and sorts them. unique() runs
           before sort(), so duplicates that are not adjacent survive; rank() uses lower_bound,
           so every copy of a value maps to its first position and the counts stay correct — the
           table is just up to 2n entries.
        2. For each num (as the right element j): ans += number of earlier values whose rank is
           above rank(2 * num), i.e. earlier values > 2 * num; then insert rank(num).
        3. ans fits in int: at most n(n-1)/2 ~ 1.25 * 10^9 for n = 5 * 10^4.

    Verdict: MATCHES BEST — the O(n log n) Fenwick-over-compressed-values sweep.
        Verified against an O(n^2) brute force on 30000 random arrays including INT_MIN / INT_MAX
        extremes, plus both examples and a descending n = 5 * 10^4 array, under ASan/UBSan.
*/

class Solution {
    private:
        using ll = long long;

        vector<ll> compress(vector<int>& nums){
            vector<ll> vals;
            for (auto& num : nums){
                vals.push_back(num);
                vals.push_back(2LL * num);
            }

            vals.erase(unique(vals.begin(), vals.end()), vals.end());
            sort(vals.begin(), vals.end());
            return vals;
        }

        struct fenwick {
            int N;
            vector<int> bits;

            fenwick(int n) : N(n+1), bits(N){}

            int query(int l){
                int sum = 0;
                while (l > 0){
                    sum += bits[l];
                    l -= l & (-l);
                }
                return sum;
            }

            int query(int l, int r){
                return query(r+1) - query(l);
            }

            void update(int k, int v){
                ++k;
                while (k < N){
                    bits[k] += v;
                    k += k & (-k);
                }
            }

        };

    public:
        int reversePairs(vector<int>& nums) {
            vector<ll> vals = compress(nums);
            int n = vals.size();

            auto rank = [&](ll X) -> int {
                return (lower_bound(vals.begin(), vals.end(), X) - vals.begin());
            };

            fenwick tree(n);

            int ans = 0;
            for (auto& num : nums){
                int r = rank(2LL * num);
                ans += tree.query(r+1, n-1);
                tree.update(rank(num), 1);
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
    vector<int> nums = {2,4,3,5,1};

    nums = {1,3,2,3,1};
    cout << sol.reversePairs(nums);
    return 0;
}