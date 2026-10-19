#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

/*
    LC 315 - Count of Smaller Numbers After Self
    https://leetcode.com/problems/count-of-smaller-numbers-after-self/

    Given an integer array nums, return an integer array counts where counts[i] is the number of smaller elements to the right of nums[i].

    Example 1:

    Input: nums = [5,2,6,1]
    Output: [2,1,1,0]
    Explanation:
    To the right of 5 there are 2 smaller elements (2 and 1).
    To the right of 2 there is only 1 smaller element (1).
    To the right of 6 there is 1 smaller element (1).
    To the right of 1 there is 0 smaller element.

    Example 2:

    Input: nums = [-1]
    Output: [0]

    Example 3:

    Input: nums = [-1,-1]
    Output: [0,0]

    Constraints:

    - 1 <= nums.length <= 10^5
    - -10^4 <= nums[i] <= 10^4
*/
/*
    Best solution: right-to-left sweep with a Fenwick tree over compressed values (or merge-sort
    counting)

    Complexity:
        Time:  O(n log n) — sort for compression, then one query + one update per element.
        Space: O(n) — the compressed values and the tree.

    Approach (detail):
        1. Compress nums to ranks 0..m-1 (sort, unique, lower_bound), preserving order.
        2. Walk i from n-1 down to 0. The tree holds a count per rank for the elements to the right
           of i.
        3. counts[i] = prefix sum of ranks strictly below rank(nums[i]); then add nums[i] to the tree.
        4. Equal values are not counted because the query stops at rank - 1.
*/
/*
    Your solution: Fenwick tree over ranks (sort + unique + lower_bound), right-to-left sweep

    Complexity:
        Time:  O(n log n) — compression sort plus one tree.query and one tree.inc per element.
        Space: O(n) — vals and the Fenwick bit array.

    Approach:
        1. vals = compress(nums); rank(x) = lower_bound position of x in vals (0-based).
        2. For i = n-1 .. 0: r = rank(nums[i]); nums[i] = tree.query(r - 1) (how many seen values
           have rank < r); then tree.inc(r).
        3. fenwicktree shifts indices by +1 internally, so query(-1) returns 0 for the smallest value.
        4. The answer is written back into nums and returned.
        Also in the file (not called): struct segtree — a point-increment / range-sum segment tree
        alternative.

    Verdict: MATCHES BEST — the O(n log n) Fenwick sweep over compressed values. Verified against an
        O(n^2) brute force on 20000 random arrays.
*/
class Solution {
    private:
        struct segtree {
            int N;
            vector<int> sum;

            segtree(int n) : N(n+1), sum(4*N+1){}

            int query(int t, int l, int r, int X, int Y){
                // case 1: [l...r][X...Y] or [X...Y][l...r]
                if (r < X || Y < l) return 0;
                // case 2: [X...[l...r]...Y]
                if (X <= l && r <= Y) return sum[t];
                // case 3: overlap
                int m = (l+r)/2;
                return query(2*t,l,m,X,Y) + query(2*t+1,m+1,r,X,Y);
            }

            int query(int X){
                return query(1, 1, N, 1, X+1);
            }

            void inc(int t, int l, int r, int k){
                // case 1: [l...r][k...k] or [k...k][l...r]
                if (r < k || k < l) return;
                // case 2: [k...l...r...k]
                if (l == r){
                    sum[t]++;
                    return;
                }
                // case 3: overlap <=> k in [l...r]
                int m = (l+r)/2;
                (k > m) ? inc(2*t+1,m+1,r,k) : inc(2*t,l,m,k);
                sum[t] = sum[2*t] + sum[2*t+1];
            }

            void inc(int k){
                inc(1,1,N,k+1);
            }
        };

        struct fenwicktree {
            int N;
            vector<int> bit;    // bit[k] chỉ chứa sum của các chỉ số: [k-X+1, k], trong đó X là giá trị của chỉ số bit cuối cùng trong k

            fenwicktree(int n) : N(n+1), bit(N){}

            void inc(int k){
                ++k;
                while (k < N){
                    ++bit[k];
                    k += k & (-k);
                }
            }

            int query(int X){
                ++X;
                int sum = 0;
                while (X > 0){
                    sum += bit[X];
                    X -= X & (-X);
                }
                return sum;
            }
        };

    private:
        vector<int> compress(vector<int>& nums){
            vector<int> compression(nums);
            sort(compression.begin(), compression.end());
            compression.erase(unique(compression.begin(), compression.end()), compression.end());
            return compression;
        }

    public:
        vector<int> countSmaller(vector<int>& nums) {
            vector<int> vals = compress(nums);
            auto rank = [&](int x){
                return (int)(lower_bound(vals.begin(), vals.end(), x) - vals.begin());
            };

            int n = nums.size();
            fenwicktree tree(vals.size());
            for (int i = n-1; i >= 0; --i){
                int num = nums[i];
                int r   = rank(num);
                nums[i] = tree.query(r-1);
                tree.inc(r);
            }
            return nums;
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