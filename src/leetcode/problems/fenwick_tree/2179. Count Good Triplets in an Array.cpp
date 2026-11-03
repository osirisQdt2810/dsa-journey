#include <vector>
#include <iostream>
#include <set>

using namespace std;

/*
    LC 2179 - Count Good Triplets in an Array
    https://leetcode.com/problems/count-good-triplets-in-an-array/

    You are given two 0-indexed arrays nums1 and nums2 of length n, both of which are permutations of [0, 1, ..., n - 1].

    A good triplet is a set of 3 distinct values which are present in increasing order by position both in nums1 and nums2. In other words, if we consider pos1_v as the index of the value v in nums1 and pos2_v as the index of the value v in nums2, then a good triplet will be a set (x, y, z) where 0 <= x, y, z <= n - 1, such that pos1_x < pos1_y < pos1_z and pos2_x < pos2_y < pos2_z.

    Return the total number of good triplets.

    Example 1:

    Input: nums1 = [2,0,1,3], nums2 = [0,1,2,3]
    Output: 1
    Explanation:
    There are 4 triplets (x,y,z) such that pos1_x < pos1_y < pos1_z. They are (2,0,1), (2,0,3), (2,1,3), and (0,1,3).
    Out of those triplets, only the triplet (0,1,3) satisfies pos2_x < pos2_y < pos2_z. Hence, there is only 1 good triplet.

    Example 2:

    Input: nums1 = [4,0,1,3,2], nums2 = [4,1,0,2,3]
    Output: 4
    Explanation: The 4 good triplets are (4,0,3), (4,0,2), (4,1,3), and (4,1,2).

    Constraints:

    - n == nums1.length == nums2.length
    - 3 <= n <= 10^5
    - 0 <= nums1[i], nums2[i] <= n - 1
    - nums1 and nums2 are permutations of [0, 1, ..., n - 1].
*/
/*
    Best solution: count "middle" values with a Fenwick tree indexed by nums2 positions

    Complexity:
        Time:  O(n log n) — one BIT query + one BIT update per element.
        Space: O(n) — pos2[] and the BIT.

    Approach (detail):
        1. Fix the middle value y of the triplet. The number of good triplets with middle y is
           left[y] * right[y], where left[y] = #values before y in BOTH arrays and
           right[y] = #values after y in BOTH arrays. Sum over all y.
        2. Map each value to its position in nums2: pos2[v].
        3. Sweep nums1 left to right (this order = "before in nums1"). Keep a BIT over the
           positions of nums2 and mark pos2[v] for every value already swept.
        4. At index i with value v, p = pos2[v]: left = BIT.prefix(p) counts the swept values
           (before in nums1) whose nums2 position is < p (before in nums2). Then mark p.
        5. right needs no second BIT: of the i swept values, i - left lie after p in nums2;
           there are n-1-p values after p in nums2, so right = (n-1-p) - (i-left).
        6. ans += left * right in 64-bit (up to ~C(10^5, 3)).
*/
/*
    Your solution: two Fenwick sweeps (forward for pairs_before, backward for pairs_after)

    Complexity:
        Time:  O(n log n) — two sweeps of n BIT query/update pairs.
        Space: O(n) — pos1/pos2, pairs_before/pairs_after and the BIT.

    Approach:
        1. Build pos2[v] (pos1 is filled too but never read).
        2. Forward sweep over nums1: pairs_before[anchor] = tree.query_sum(pos2[anchor]) — the
           1-based prefix up to pos2 covers 0-based slots [0, pos2) — then tree.inc(pos2+1).
        3. tree.clear(); backward sweep over nums1: the tree holds the n-1-i values after i in
           nums1, query_sum(pos2[anchor]+1) counts those before anchor in nums2 (anchor itself
           is not inserted yet), so pairs_after[anchor] = (n-i-1) - that count.
        4. Sum 1LL * pairs_before[T] * pairs_after[T] over all T.

    Verdict: MATCHES BEST — same O(n log n) / O(n); the second sweep (and the unused pos1) are
        only a constant factor, replaceable by right = (n-1-p) - (i-left).
        Verified against an O(n^3) brute force on 3000 random permutation pairs, n in [3, 10].
*/
class Solution {
    public:
        long long goodTriplets(vector<int>& nums1, vector<int>& nums2) {
            // ta biến đổi bài toán về như sau:
            // giả sử tại giá trị T, ta tìm được:
            //  - t1, t2: vị trí của T trong 2 mảng
            //  - trước t1 và t2 ở mỗi mảng có bao nhiêu cặp trùng nhau => X
            //  - sau t1 và t2 ở mỗi mảng có bao nhiêu cặp trùng nhau => Y
            // vậy:
            //  - số cặp thoả mãn với T là giá trị nằm giữa là: X*Y
            //  - sum tất cả T từ 1->N ta được kết quả

            // vậy làm sao để tìm nhanh được `trước t1 và t2 ở mỗi mảng có bao nhiêu cặp trùng nhau => X`
            // giả sử ta duyệt mảng 1 theo i tăng dần:
            //  - với mỗi i đã được duyệt, ta add mark vào 1 flags boolean ~ mảng 2
            //  - khi ta có t2, ta chỉ việc kiểm tra xem sum(mark) từ [0->t2) là bao nhiêu => đó chính là X

            int n = nums1.size();
            vector<int> pos1(n, -1), pos2(n, -1);
            vector<int> pairs_before(n, 0);
            vector<int> pairs_after(n, 0);

            struct fenwicktree {
                int N;
                vector<int> bits;

                fenwicktree(int n) : N(n+1), bits(N, 0){}

                void clear(){
                    bits.assign(N, 0);
                }

                int query_sum(int k){
                    int sum = 0;
                    while (k > 0){
                        sum += bits[k];
                        k -= k & (-k);
                    }
                    return sum;
                }

                void inc(int k){
                    while (k < N){
                        ++bits[k];
                        k += k & (-k);
                    }
                }
            };

            for (int i = 0; i < n; ++i){
                pos1[nums1[i]] = i;
                pos2[nums2[i]] = i;
            }

            fenwicktree tree(n);
            for (int i = 0; i < n; ++i){
                int anchor = nums1[i];
                pairs_before[anchor] = tree.query_sum(pos2[anchor]);
                tree.inc(pos2[anchor]+1);
            }

            tree.clear();
            for (int i = n-1; i >= 0; --i){
                // nums1 = [4,0,1,3,2]
                // nums2 = [4,1,0,2,3]
                int anchor = nums1[i];
                pairs_after[anchor] = (n-i-1) - tree.query_sum(pos2[anchor]+1); //
                tree.inc(pos2[anchor]+1);
            }

            long long sum = 0;
            for (int T = 0; T < n; ++T){
                sum += 1LL * pairs_before[T] * pairs_after[T];
            }
            return sum;
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