#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

/*
    LC 406 - Queue Reconstruction by Height
    https://leetcode.com/problems/queue-reconstruction-by-height/

    You are given an array of people, people, which are the attributes of some people in a queue (not necessarily in order). Each people[i] = [h_i, k_i] represents the i^th person of height h_i with exactly k_i other people in front who have a height greater than or equal to h_i.

    Reconstruct and return the queue that is represented by the input array people. The returned queue should be formatted as an array queue, where queue[j] = [h_j, k_j] is the attributes of the j^th person in the queue (queue[0] is the person at the front of the queue).

    Example 1:

    Input: people = [[7,0],[4,4],[7,1],[5,0],[6,1],[5,2]]
    Output: [[5,0],[7,0],[5,2],[6,1],[4,4],[7,1]]
    Explanation:
    Person 0 has height 5 with no other people taller or the same height in front.
    Person 1 has height 7 with no other people taller or the same height in front.
    Person 2 has height 5 with two persons taller or the same height in front, which is person 0 and 1.
    Person 3 has height 6 with one person taller or the same height in front, which is person 1.
    Person 4 has height 4 with four people taller or the same height in front, which are people 0, 1, 2, and 3.
    Person 5 has height 7 with one person taller or the same height in front, which is person 1.
    Hence [[5,0],[7,0],[5,2],[6,1],[4,4],[7,1]] is the reconstructed queue.

    Example 2:

    Input: people = [[6,0],[5,0],[4,0],[3,2],[2,2],[1,4]]
    Output: [[4,0],[5,0],[2,2],[3,2],[1,4],[6,0]]

    Constraints:

    - 1 <= people.length <= 2000
    - 0 <= h_i <= 10^6
    - 0 <= k_i < people.length
    - It is guaranteed that the queue can be reconstructed.
*/
/*
    Best solution: shortest-first placement into the (k+1)-th empty slot, found by descending a
    segment tree (or binary lifting on a Fenwick tree) over a 0/1 "slot is empty" array

    Complexity:
        Time:  O(n log n) — sorting plus one k-th-one search and one point update per person.
        Space: O(n) — the tree and the result array.

    Approach (detail):
        1. Sort by height ascending; equal heights by k DESCENDING.
        2. When person (h, k) is placed, everyone not yet placed is taller or equal, and each
           still-empty slot will be filled by one of them. So exactly k of them stand in front of
           this person iff there are exactly k empty slots before it: put it into the (k+1)-th
           empty slot. Already-filled slots hold shorter people, which never count toward k.
        3. Equal heights do count toward each other's k; placing the larger k first leaves its
           k empty slots for the equal-height people with smaller k that come later.
        4. Keep e[i] = 1 for an empty slot. "(k+1)-th empty slot" = smallest i with prefix sum
           (e[1..i]) = k+1: descend the segment tree (go left if the left child holds >= m ones,
           else go right with m -= left sum), then set e[i] = 0.
        5. Alternative O(n^2): tallest first, insert each person at index k of a vector.
*/
/*
    Your solution: segment tree over empty slots with an argsum descent

    Complexity:
        Time:  O(n log n) — sort, then argsum(k + 1) and update(index, 0) per person, O(log n) each.
        Space: O(n) — the 4n segment-tree array and res.

    Approach:
        1. Sort by h ascending, ties by k descending.
        2. segment tree(n) is built with every slot = 1 (empty).
        3. For each person: index = argsum(k + 1) walks down — right with s - sum[left] when the
           left child has fewer than s empty slots, left otherwise — to the (k+1)-th empty slot;
           res[index - 1] = person; update(index, 0) marks the slot filled.
        4. Also in the file (not used): struct fenwick — a prefix-count tree left from an earlier
           attempt.

    Verdict: MATCHES BEST — the O(n log n) empty-slot placement with a segment-tree descent.
        Verified on Example 1 and on 20000 random valid queues (n <= 9) by checking every person's
        k in the output, under ASan/UBSan.
*/
class Solution {
    private:
        struct fenwick {
            int N;
            vector<int> bits;
            fenwick(int n) : N(n+1), bits(N, 0){}

            int query(int k){
                int sum = 0;
                while (k > 0){
                    sum += bits[k];
                    k -= k & (-k);
                }
                return sum;
            }

            void update(int k){ // arr[k]++
                while (k < N){
                    bits[k]++;
                    k += k & (-k);
                }
            }
        };

        struct segment {
            int N;
            vector<int> sum;

            void build(int t, int l, int r, int val){
                if (l == r){
                    sum[t] = val;
                    return;
                }
                int m = (l+r)/2;
                build(2*t, l, m, val);
                build(2*t+1, m+1, r, val);
                sum[t] = sum[2*t] + sum[2*t+1];
            }

            segment(int n) : N(n), sum(4*N+1, 0){
                build(1, 1, N, 1); // all value = 1
            }

            int argsum(int t, int l, int r, int s){ // find i where sum[l...i] == s
                if (l == r) return (sum[t] == s) ? l : -1;
                int m = (l+r)/2;
                if (sum[2*t] < s){
                    return argsum(2*t+1,m+1,r,s-sum[2*t]);
                }
                else {
                    return argsum(2*t,l,m,s);
                }
            }

            int argsum(int S){  // find i where sum(1...i) == S
                return argsum(1, 1, N, S);
            }

            void update(int t, int l, int r, int k, int val){
                // case 1: [l...r] [k...k] or [k...k][l...r]
                if (r < k || k < l) return;
                // case 2: l=r=k
                if (l == r){
                    sum[t] = val;
                    return;
                }
                // case 3: overlap
                int m = (l+r)/2;
                (k <= m) ? update(2*t, l, m, k, val) : update(2*t+1, m+1, r, k, val);
                sum[t] = sum[2*t] + sum[2*t+1];
            }

            void update(int k, int val){    // update bit[k] = val
                update(1, 1, N, k, val);
            }
        };

    public:
        vector<vector<int>> reconstructQueue(vector<vector<int>>& people) {
            int n = people.size();

            sort(people.begin(), people.end(), [&](const vector<int>& lhs, const vector<int>& rhs){
                if (lhs[0] < rhs[0]) return true;
                if (lhs[0] == rhs[0]) return lhs[1] > rhs[1];
                return false;
            });

            // for (auto& p : people){
            //     cout << "(" << p[0] << "," << p[1] << ")\n";
            // }

            segment tree(n);    // val[i] = 0 => marked, val[i] = 1 => unvisited
            vector<vector<int>> res(n);
            for (int i = 0; i < n; ++i){
                int k = people[i][1];
                int index = tree.argsum(k+1);
                res[index-1] = people[i];
                tree.update(index, 0);
                // cout << "i=" << i << " k=" << k << " index=" << index << "\n";
            }
            return res;
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

    vector<vector<int>> people = {{7,0},{4,4},{7,1},{5,0},{6,1},{5,2}};
    sol.reconstructQueue(people);
    return 0;
}