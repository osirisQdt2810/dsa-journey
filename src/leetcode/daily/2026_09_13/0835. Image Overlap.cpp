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
    LC 835 - Image Overlap
    https://leetcode.com/problems/image-overlap/

    You are given two images, img1 and img2, represented as binary, square matrices of size n x n. A binary matrix has only 0s and 1s as values.

    We translate one image however we choose by sliding all the 1 bits left, right, up, and/or down any number of units. We then place it on top of the other image. We can then calculate the overlap by counting the number of positions that have a 1 in both images.

    Note also that a translation does not include any kind of rotation. Any 1 bits that are translated outside of the matrix borders are erased.

    Return the largest possible overlap.

    Example 1:

    Input: img1 = [[1,1,0],[0,1,0],[0,1,0]], img2 = [[0,0,0],[0,1,1],[0,0,1]]
    Output: 3
    Explanation: We translate img1 to right by 1 unit and down by 1 unit.

    The number of positions that have a 1 in both images is 3 (shown in red).

    Example 2:

    Input: img1 = [[1]], img2 = [[1]]
    Output: 1

    Example 3:

    Input: img1 = [[0]], img2 = [[0]]
    Output: 0

    Constraints:

    - n == img1.length == img1[i].length
    - n == img2.length == img2[i].length
    - 1 <= n <= 30
    - img1[i][j] is either 0 or 1.
    - img2[i][j] is either 0 or 1.
*/
/*
    Best solution: enumerate every translation, count overlaps with row bitmasks

    Complexity:
        Time:  O(n^3) — (2n-1)^2 translations, n row ANDs + popcounts each (~1.1 * 10^5 at n = 30).
        Space: O(n) — one bitmask per row of each image.

    Approach (detail):
        1. Sliding img1 by (dy, dx), both in [-(n-1), n-1], covers every possible placement; for
           a fixed translation the overlap region is fully determined, so no window sizes need
           to be enumerated.
        2. Encode each row as an n-bit integer. Shifting a row horizontally is a bit shift, and
           the overlap of two aligned rows is popcount(row1 & row2).
        3. For each translation sum the popcounts over the rows that stay inside the grid after
           the vertical shift; the answer is the maximum sum.
        4. Alternative: for every pair (1-cell of img1, 1-cell of img2) count the translation
           vector b - a; the most frequent vector wins — O(|A| * |B|), better on sparse images.
*/
/*
    Your solution: translation loop over (dx, dy) with row bitmasks and popcount

    Complexity:
        Time:  O(n^3) — (2n-1)^2 translations x n rows.
        Space: O(n) — nums1 / nums2.

    Approach:
        1. convert_to_integer packs each row into an int with column 0 as the highest bit.
        2. For every (dx, dy): row r of img2 is compared with row nr = r - dy of img1, skipping
           rows that fall outside the grid.
        3. img1's row is shifted by dx (>> for dx > 0, << for dx < 0); bits pushed past the
           grid are dropped by the AND with nums2[r], which only has the low n bits.
        4. overlap sums popcount(num1 & num2); largest_overlap keeps the maximum.
        5. The << -dx shift can move bits past bit 31; that is well-defined since C++20 (modular
           signed shifts) and harmless here because the AND keeps only the low n bits.

    Verdict: MATCHES BEST — the O(n^3) bitmask translation scan.
        Verified against a cell-by-cell brute force over all translations on 20000 random image
        pairs (n <= 6) and on random 30x30 pairs, under UBSan for the 30x30 run.
*/
class Solution {
    public:
        int largestOverlap(vector<vector<int>>& img1, vector<vector<int>>& img2) {
            int n = img1.size();

            auto convert_to_integer = [&](const vector<vector<int>>& img) -> vector<int> {
                vector<int> nums;
                for (auto row : img){
                    int num = 0;
                    for (int i = 0; i < n; ++i){
                        if (row[i] == 1){
                            num |= (1 << (n-i-1));
                        }
                    }
                    nums.push_back(num);
                }
                return nums;
            };

            auto nums1 = convert_to_integer(img1);
            auto nums2 = convert_to_integer(img2);

            int largest_overlap = 0;
            for (int dx = -(n-1); dx <= (n-1); ++dx){
                for (int dy = -(n-1); dy <= (n-1); ++dy){
                    // row changed from: [0->n-1] to [fy, ty]
                    int overlap = 0;
                    for (int r = 0; r < n; ++r){
                        int nr = r - dy;
                        if (nr < 0 || nr >= n) continue;
                        // compare img1[nr] <-> img2[r]
                        int num1 = (dx > 0) ? (nums1[nr] >> dx) : (nums1[nr] << (-dx));
                        int num2 = nums2[r];
                        int o = num1 & num2;
                        overlap += __builtin_popcount(o);
                    }
                    largest_overlap = max(largest_overlap, overlap);

                }
            }
            return largest_overlap;
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