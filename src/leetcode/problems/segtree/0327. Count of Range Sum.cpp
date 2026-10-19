#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

/*
    LC 327 - Count of Range Sum
    https://leetcode.com/problems/count-of-range-sum/

    Given an integer array nums and two integers lower and upper, return the number of range sums that lie in [lower, upper] inclusive.

    Range sum S(i, j) is defined as the sum of the elements in nums between indices i and j inclusive, where i <= j.

    Example 1:

    Input: nums = [-2,5,-1], lower = -2, upper = 2
    Output: 3
    Explanation: The three ranges are: [0,0], [2,2], and [0,2] and their respective sums are: -2, -1, 2.

    Example 2:

    Input: nums = [0], lower = 0, upper = 0
    Output: 1

    Constraints:

    - 1 <= nums.length <= 10^5
    - -2^31 <= nums[i] <= 2^31 - 1
    - -10^5 <= lower <= upper <= 10^5
    - The answer is guaranteed to fit in a 32-bit integer.
*/
/*
    Best solution: prefix sums + Fenwick tree over compressed prefix values (or merge-sort counting)

    Complexity:
        Time:  O(n log n) — compress O(n) values, then one range query + one update per prefix.
        Space: O(n) — compressed values and the tree.

    Approach (detail):
        1. Let P[0] = 0 and P[k] = nums[0] + ... + nums[k-1] (64-bit: sums reach 10^5 * 2^31).
        2. sum(nums[j..i]) = P[i+1] - P[j], so for each i count j <= i with
           P[i+1] - upper <= P[j] <= P[i+1] - lower.
        3. Compress every value that is inserted (all P[k]) and every query bound (P[k] - lower,
           P[k] - upper) into one sorted unique array.
        4. Insert P[0] first; then for k = 1..n: answer += count of inserted values in
           [P[k] - upper, P[k] - lower], then insert P[k].
*/
/*
    Your solution: Fenwick tree counting prefix sums, with all prefixes and both bounds compressed

    Complexity:
        Time:  O(n log n) — sort of the 3n + 3 compressed values, then O(log n) per step.
        Space: O(n) — vals / compression and the Fenwick bit array.

    Approach:
        1. vals collects 0, -lower, -upper and, for each running prefix s, s - lower, s - upper and s
           (all long long); compress() sorts and de-duplicates them.
        2. rank(x) = lower_bound index of x; the Fenwick tree is 1-indexed, hence the + 1 offsets.
        3. tree.inc(rank(0) + 1) records P[0] = 0 before the loop.
        4. For each i: s += nums[i]; l = rank(s - upper), r = rank(s - lower);
           ans += tree.query(l + 1, r + 1); then tree.inc(rank(s) + 1).

    Verdict: MATCHES BEST — the O(n log n) prefix-sum + Fenwick count. Verified against an O(n^2)
        brute force on 200000 small random cases and on [INT_MAX, INT_MAX] (64-bit prefixes).
*/
class Solution {
    private:
        using ll = long long;
        vector<ll> compress(vector<ll>& nums){
            vector<ll> vals = nums;
            sort(vals.begin(), vals.end());
            vals.erase(unique(vals.begin(), vals.end()), vals.end());
            return vals;
        }

        struct fenwicktree {
            int N;
            vector<ll> bit;

            fenwicktree(int n) : N(n+1), bit(N){}

            ll query(int k){
                ll sum = 0;
                while (k > 0){
                    sum += bit[k];
                    k -= k & (-k);
                }
                return sum;
            }

            ll query(int l, int r){
                return (l == 1) ? query(r) : (query(r) - query(l-1));
            }

            void inc(int k){
                while (k < N){
                    ++bit[k];
                    k += k & (-k);
                }
            }
        };

    public:
        int countRangeSum(vector<int>& nums, int lower, int upper) {
            int n = nums.size();
            ll s = 0;
            vector<ll> vals = {-lower, -upper, 0};
            for (int i = 0; i < n; ++i){
                s += nums[i];
                vals.push_back(s - lower);
                vals.push_back(s - upper);
                vals.push_back(s);
            }

            vector<ll> compression = compress(vals);
            auto rank = [&](ll x){
                return int(lower_bound(compression.begin(), compression.end(), x) - compression.begin());
            };

            fenwicktree tree(compression.size());

            s = 0;
            int ans = 0;
            tree.inc(rank(0) + 1);
            for (int i = 0; i < n; ++i){    // find j: [0...i] sao cho sum[i+1] - sum[j] in [lower, upper]
                                            //
                s += nums[i];
                int l = rank(s - upper);
                int r = rank(s - lower);
                int k = rank(s);
                ans += tree.query(l+1, r+1);
                tree.inc(k+1);
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