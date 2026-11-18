#include <vector>
#include <iostream>
#include <string>
#include <deque>
#include <algorithm>

using namespace std;

/*
    LC 3578 - Count Partitions With Max-Min Difference at Most K
    https://leetcode.com/problems/count-partitions-with-max-min-difference-at-most-k/
    @status: hint
    @time: 979
    @wa: 1
    @created: 2026-11-17 17:22

    You are given an integer array nums and an integer k. Your task is to partition nums into one or more non-empty contiguous segments such that in each segment, the difference between its maximum and minimum elements is at most k.

    Return the total number of ways to partition nums under this condition.

    Since the answer may be too large, return it modulo 10^9 + 7.

    Example 1:

    Input: nums = [9,4,1,3,7], k = 4
    Output: 6
    Explanation:
    There are 6 valid partitions where the difference between the maximum and minimum elements in each segment is at most k = 4:
    - [[9], [4], [1], [3], [7]]
    - [[9], [4], [1], [3, 7]]
    - [[9], [4], [1, 3], [7]]
    - [[9], [4, 1], [3], [7]]
    - [[9], [4, 1], [3, 7]]
    - [[9], [4, 1, 3], [7]]

    Example 2:

    Input: nums = [3,3,4], k = 0
    Output: 2
    Explanation:
    There are 2 valid partitions that satisfy the given conditions:
    - [[3], [3], [4]]
    - [[3, 3], [4]]

    Constraints:

    - 2 <= nums.length <= 5 * 10^4
    - 1 <= nums[i] <= 10^9
    - 0 <= k <= 10^9
*/
/*
    Best solution: prefix DP with a sliding window of valid segment starts and two monotonic queues

    Complexity:
        Time:  O(n) — every element is pushed to / popped from each queue once, and l only moves right.
        Space: O(n) — the dp array and the two deques.

    Approach (detail):
        1. P[i] = number of ways to split the prefix nums[0..i-1]; P[0] = 1 (the empty prefix).
        2. If the last segment is nums[j..r], it is valid iff max - min <= k, and the rest nums[0..j-1]
           can be split in P[j] ways. So P[r+1] = sum of P[j] over the valid starts j.
        3. A segment only gets worse when it grows to the left, so the valid starts form a window
           [l..r] whose left end only moves right as r grows.
        4. Keep the window's max and min with two monotonic deques; while max - min > k pop the front of
           both and advance l. Keep S = sum of P[j] for j in [l..r] and update it incrementally
           (add P[r] when r joins, subtract P[l] when l leaves), all modulo 10^9 + 7.
        5. P[r+1] = S, and the answer is P[n].
*/
/*
    Your solution: prefix DP + sliding window of starts + hand-written minqueue / maxqueue
        (live: sol_by_changing_order_a_little_bit)

    Complexity:
        Time:  O(n) — amortized, as above.
        Space: O(n) — P[n+1] and the deques.

    Approach:
        1. minqueue / maxqueue hold a deque of candidates (monotonic) and a deque `arr` of the window's
           elements; pop() drops arr.front() and also candidates.front() when they are equal.
        2. P[0] = 1, S = 0, l = 0. For each r: push nums[r] into both queues, S += P[r] (the start j = r),
           then while max - min > k: pop both queues, S -= P[l], ++l.
        3. P[r+1] = S (mod 10^9 + 7); return P[n].
        Also in the file (not called): sol_straight_forward — the same DP indexed as P[r] over
        nums[0..r], with before(l) standing for P[l-1] and P(-1) = 1.

    Verdict: MATCHES BEST — the O(n) prefix DP with monotonic queues. Verified: 2/2 official examples and a
        brute force over every split (O(n^3)) on 100000 random arrays (n <= 13, values 1..10, k <= 5),
        plus n = 50000 with values up to 10^9.
        Note: the DP indexing and the bug fixes were worked out with Claude's help (@status: hint).
*/
class Solution {
    public:
        int countPartitions(vector<int>& nums, int k) {
            using ll = long long;
            const int MOD = 1e9 + 7;
            auto sum_mod = [](int a, int b) -> int {
                int c = a + b;
                if (c >= MOD) c -= MOD;
                return c;
            };

            struct minqueue {
                deque<int> candidates;
                deque<int> arr;

                void push(int val){
                    while (!candidates.empty() && candidates.back() > val) candidates.pop_back();
                    candidates.push_back(val);
                    arr.push_back(val);
                }

                void pop(){
                    if (arr.empty()) return;
                    if (arr.front() == candidates.front()) candidates.pop_front();
                    arr.pop_front();
                }

                int front(){
                    return candidates.front();
                }
            };

            struct maxqueue {
                deque<int> candidates;
                deque<int> arr;
                void push(int val){
                    while (!candidates.empty() && candidates.back() < val) candidates.pop_back();
                    candidates.push_back(val);
                    arr.push_back(val);
                }

                void pop(){
                    if (arr.empty()) return;
                    if (arr.front() == candidates.front()) candidates.pop_front();
                    arr.pop_front();
                }

                int front(){
                    return candidates.front();
                }
            };

            minqueue min_queue;
            maxqueue max_queue;
            int n = nums.size();

            auto sol_by_changing_order_a_little_bit = [&]() -> int {
                vector<int> P(n+1, 0);
                int S = 0;
                P[0] = 1;

                // [9,4,1,3,7], k = 4
                // r=0~9:
                //  queue=[9]
                //  parts=[1]
                //  sum=2, l=0, sum có nghĩa là tổng từ [l-1,r]
                // r=1~4:
                //  queue=[4]
                //  sum=2, l=1
                //  parts=[1,2]
                //  sum=4, l=0, [-1,1]
                // r=2~1
                //  queue=[4,1]
                //  parts=[1,2,4]
                // min_queue.push(nums[0]);
                // max_queue.push(nums[0]);
                int l = 0;

                for (int r = 0; r < n; ++r){
                    min_queue.push(nums[r]);
                    max_queue.push(nums[r]);
                    // cộng r-1
                    S = (S + P[r]) % MOD;

                    while (max_queue.front() - min_queue.front() > k){
                        max_queue.pop();
                        min_queue.pop();
                        S = (S - P[l] + MOD) % MOD;
                        ++l;
                    }
                    P[r+1] = S;
                    // cout << "r="  << r << " -> l=" << l << " sum_before=" << sum_partitions << ", parts=" << partitions[r] << " , sum_after=" << sum_partitions << "\n";
                }
                return P[n];
            };

            auto sol_straight_forward = [&]() -> int {
                vector<int> P(n, 0);
                int S = 1;
                int l = 0;

                auto before = [&](int l) -> int {
                    return (l == 0) ? 1 : P[l-1];
                };

                for (int r = 0; r < n; ++r){
                    min_queue.push(nums[r]);
                    max_queue.push(nums[r]);
                    while (max_queue.front() - min_queue.front() > k){
                        max_queue.pop();
                        min_queue.pop();
                        S = (S - before(l) + MOD) % MOD;
                        ++l;
                    }
                    P[r] = S;
                    S = (S + P[r]) % MOD;
                }

                return P[n-1];
            };

            return sol_by_changing_order_a_little_bit();
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
        vector<int> nums = {9, 4, 1, 3, 7};
        int k = 4;
        check(1, sol.countPartitions(nums, k), 6);
    }
    {
        Solution sol;
        vector<int> nums = {3, 3, 4};
        int k = 0;
        check(2, sol.countPartitions(nums, k), 2);
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}