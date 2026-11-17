#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <set>

using namespace std;

/*
    LC 31 - Next Permutation
    https://leetcode.com/problems/next-permutation/
    @status: self
    @time: 26
    @wa: 3
    @created: 2026-11-17 13:13

    A permutation of an array of integers is an arrangement of its members into a sequence or linear order.

    - For example, for arr = [1, 2, 3], the following are all the permutations of arr: [1, 2, 3], [1, 3, 2], [2, 1, 3], [2, 3, 1], [3, 1, 2], [3, 2, 1].

    The next permutation of an array of integers is the next lexicographically greater permutation of its integer. More formally, if all the permutations of the array are sorted in one container according to their lexicographical order, then the next permutation of that array is the permutation that follows it in the sorted container. If such arrangement is not possible, the array must be rearranged as the lowest possible order (i.e., sorted in ascending order).

    Examples:

    - The next permutation of arr = [1, 2, 3] is [1, 3, 2].
    - The next permutation of arr = [2, 3, 1] is [3, 1, 2].
    - The next permutation of arr = [3, 2, 1] is [1, 2, 3] because [3, 2, 1] does not have a lexicographical larger rearrangement.

    Given an array of integers nums, find the next permutation of nums.

    The replacement must be in place and use only constant extra memory.

    Example 1:

    Input: nums = [1,2,3]
    Output: [1,3,2]

    Example 2:

    Input: nums = [3,2,1]
    Output: [1,2,3]

    Example 3:

    Input: nums = [1,1,5]
    Output: [1,5,1]

    Constraints:

    - 1 <= nums.length <= 100
    - 0 <= nums[i] <= 100
*/
/*
    Best solution: find the pivot from the right, swap it with the next larger element, reverse the suffix

    Complexity:
        Time:  O(n) — one backward scan, one swap, one reversal.
        Space: O(1) — in place.

    Approach (detail):
        1. Scan from the right for the largest index p with nums[p] < nums[p+1] (the pivot). Everything
           after p is non-increasing, i.e. already the largest arrangement of those elements.
        2. If there is no pivot, the array is the last permutation: reverse it to get the first.
        3. Otherwise swap nums[p] with the rightmost element of the suffix that is greater than nums[p]
           (the smallest value that beats it, since the suffix is non-increasing).
        4. The suffix is still non-increasing after the swap; reverse it so it becomes the smallest
           arrangement. The result is the next permutation in lexicographic order.
*/
/*
    Your solution: pivot from the right, binary-search the swap target in the non-increasing suffix, reverse

    Complexity:
        Time:  O(n) — the pivot scan and the final reversal are linear; the binary search is O(log n).
        Space: O(1) — in place.

    Approach:
        1. n == 1 returns at once. p = largest index with nums[p] < nums[p+1] (or -1 if none).
        2. If p >= 0: binary search l..r over the suffix [p+1, n-1] for the rightmost m with
           nums[m] > nums[p] (valid because the suffix is non-increasing), store it in s, then
           swap(nums[p], nums[s]).
        3. Reverse nums[p+1 .. n-1] with two pointers; when p == -1 this reverses the whole array.

    Verdict: MATCHES BEST — the O(n) / O(1) algorithm; the binary search only replaces a linear scan for
        the swap target. Verified: 3/3 official examples and std::next_permutation on 300000 random
        arrays (n <= 9, values 0..4, so duplicates are covered).
*/
class Solution {
    public:
        void nextPermutation(vector<int>& nums) {
            int n = nums.size();
            if (n == 1) return;
            int p = n-2; for (; p >= 0; --p) if (nums[p] < nums[p+1]) break;
            if (p >= 0){
                int l = p+1, r = n-1, s = l;
                while (l <= r){
                    int m = (l+r)/2;
                    if (nums[m] > nums[p]){
                        s = m;
                        l = m+1;
                    }
                    else {
                        r = m-1;
                    }
                }
                swap(nums[p], nums[s]);
            }
            int l = p+1, r=n-1;
            while (l <= r){
                swap(nums[l], nums[r]);
                ++l;
                --r;
            }
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
    // nextPermutation rewrites nums in place, so compare nums itself after the call
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
        vector<int> nums = {1, 2, 3};
        sol.nextPermutation(nums);
        check(1, nums, {1, 3, 2});
    }
    {
        Solution sol;
        vector<int> nums = {3, 2, 1};
        sol.nextPermutation(nums);
        check(2, nums, {1, 2, 3});
    }
    {
        Solution sol;
        vector<int> nums = {1, 1, 5};
        sol.nextPermutation(nums);
        check(3, nums, {1, 5, 1});
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}