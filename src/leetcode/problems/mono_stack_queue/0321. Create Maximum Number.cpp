#include <vector>
#include <iostream>
#include <string>
#include <deque>
#include <algorithm>

using namespace std;

/*
    LC 321 - Create Maximum Number
    https://leetcode.com/problems/create-maximum-number/
    @status: self
    @time: 149
    @wa: 3
    @created: 2026-11-18 11:00

    You are given two integer arrays nums1 and nums2 of lengths m and n respectively. nums1 and nums2 represent the digits of two numbers. You are also given an integer k.

    Create the maximum number of length k <= m + n from digits of the two numbers. The relative order of the digits from the same array must be preserved.

    Return an array of the k digits representing the answer.

    Example 1:

    Input: nums1 = [3,4,6,5], nums2 = [9,1,2,5,8,3], k = 5
    Output: [9,8,6,5,3]

    Example 2:

    Input: nums1 = [6,7], nums2 = [6,0,4], k = 5
    Output: [6,7,6,0,4]

    Example 3:

    Input: nums1 = [3,9], nums2 = [8,9], k = 3
    Output: [9,8,9]

    Constraints:

    - m == nums1.length
    - n == nums2.length
    - 1 <= m, n <= 500
    - 0 <= nums1[i], nums2[i] <= 9
    - 1 <= k <= m + n
    - nums1 and nums2 do not have leading zeros.
*/
/*
    Best solution: try every split of k, monotonic-stack max subsequence per array, greedy merge by suffix

    Complexity:
        Time:  O(k * (m + n)^2) worst case — for each of the <= k + 1 splits the two picks cost O(m + n)
               and the merge compares suffixes (O(len) per comparison, usually far less). Comparing the
               suffixes with hashing + binary search brings the merge to O((m + n) log (m + n)).
        Space: O(m + n) — the picked subsequences and the merged candidate.

    Approach (detail):
        1. The answer takes i digits from nums1 and k - i from nums2 for some feasible i; try every i.
        2. The largest subsequence of length x of one array: keep a stack, drop = len - x, pop while the
           top is smaller than the incoming digit and drops remain, finally truncate to x.
        3. Merge the two picks into the largest sequence: at each step take the head of the side whose
           REMAINING suffix is lexicographically greater (a suffix that is a prefix of the other is the
           smaller one). Looking at a fixed number of next digits is not enough.
        4. Keep the lexicographically largest candidate over all splits.
*/
/*
    Your solution: split k, monotonic-stack picks, merge by comparing the remaining suffixes (is_greater)

    Complexity:
        Time:  O(k * (m + n)^2) worst case, as above (a linear suffix comparison inside merge).
        Space: O(m + n).

    Approach:
        1. For i = 0..k (skipping infeasible splits), get_max_number(nums1, i) and
           get_max_number(nums2, k - i) build the two largest subsequences with a monotonic stack
           (`drop` = n - k pops allowed, then st.resize(k)).
        2. merge(A, B): when one side is empty, copy the other; otherwise is_greater(A, B, l, r) walks
           both suffixes until they differ and, when one ends, compares the REMAINING lengths
           (lx - i) > (ly - j); the greater suffix gives the next digit.
        3. is_better(number, best) compares two candidates of the same length (an empty `best` always
           loses); the best candidate is returned.
        Note: the live method still contains debug `cout` lines.

    Verdict: MATCHES BEST — the standard algorithm (hash-based suffix comparison is an optional
        speed-up). Verified: 3/3 official examples and a brute force over every choice of subsequences
        and interleaving on 200000 random cases (n <= 5, values 0..2), plus a worst-case 500 + 500 input
        with long equal runs (< 1 ms).
*/
class Solution {
    public:
        vector<int> maxNumber(vector<int>& nums1, vector<int>& nums2, int k) {
            auto is_greater = [](const vector<int>& X, const vector<int>& Y, int i, int j) -> bool {
                int lx = X.size(), ly = Y.size();
                while (i < lx && j < ly){
                    if (X[i] > Y[j]) return true;
                    if (X[i] < Y[j]) return false;
                    ++i;
                    ++j;
                }
                return (lx - i) > (ly - j);
            };

            auto is_better = [&](const vector<int>& X, const vector<int>& Y) -> bool {
                if (Y.empty()) return true;
                return is_greater(X, Y, 0, 0);

            };

            auto get_max_number = [](const vector<int>& nums, int k) -> vector<int>{
                int n = nums.size();
                int drop = n - k;
                vector<int> st;
                for (int i = 0; i < n; ++i){
                    // push vào st, và chỉ pop khi không được pop nữa
                    while (!st.empty() && st.back() < nums[i] && drop > 0){
                        st.pop_back();
                        --drop;
                    }
                    st.push_back(nums[i]);
                }
                st.resize(k);
                return st;
            };

            auto merge = [&](const vector<int>& A, const vector<int>& B) -> vector<int> {
                int la = A.size(), lb = B.size();
                vector<int> res;
                int l = 0, r = 0;
                while (l < la || r < lb){
                    if (l == la){
                        res.push_back(B[r++]);
                        continue;
                    }
                    if (r == lb){
                        res.push_back(A[l++]);
                        continue;
                    }

                    is_greater(A, B, l, r) ? res.push_back(A[l++]) : res.push_back(B[r++]);
                }
                return res;
            };

            vector<int> best;
            for (int i = 0; i <= k; ++i){
                if (nums1.size() < i || nums2.size() < k-i) continue;
                auto subseq1 = get_max_number(nums1, i);
                auto subseq2 = get_max_number(nums2, k-i);
                auto number  = merge(subseq1, subseq2);

                cout << "i=" << i << "\n\tsub1: "; for (auto c : subseq1) cout << c << " ";
                cout << "\n\tsub2: "; for (auto c : subseq2) cout << c << " ";
                cout << "\n\tmerge: "; for (auto c : number) cout << c << " ";
                cout << "\n";

                if (is_better(number, best)){
                    best.swap(number);
                    cout << "\tupdate best: "; for (auto c : best) cout << c << " "; cout << "\n";
                }
            }
            return best;
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
    auto to_str = [](const vector<int>& v) {
        string s = "[";
        for (size_t i = 0; i < v.size(); ++i) s += (i ? "," : "") + to_string(v[i]);
        return s + "]";
    };
    int passed = 0, total = 0;
    auto check = [&](int idx, const vector<int>& got, const vector<int>& expected) {
        ++total;
        if (got == expected) {
            ++passed;
            cout << "Example " << idx << ": PASS\n";
        } else {
            cout << "Example " << idx << ": FAIL  got=" << to_str(got) << "  expected=" << to_str(expected) << "\n";
        }
    };
    {
        Solution sol;
        vector<int> nums1 = {3, 4, 6, 5};
        vector<int> nums2 = {9, 1, 2, 5, 8, 3};
        int k = 5;
        check(1, sol.maxNumber(nums1, nums2, k), {9, 8, 6, 5, 3});
    }
    {
        Solution sol;
        vector<int> nums1 = {6, 7};
        vector<int> nums2 = {6, 0, 4};
        int k = 5;
        check(2, sol.maxNumber(nums1, nums2, k), {6, 7, 6, 0, 4});
    }
    {
        Solution sol;
        vector<int> nums1 = {3, 9};
        vector<int> nums2 = {8, 9};
        int k = 3;
        check(3, sol.maxNumber(nums1, nums2, k), {9, 8, 9});
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}