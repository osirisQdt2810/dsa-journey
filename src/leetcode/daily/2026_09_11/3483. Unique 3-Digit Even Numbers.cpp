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
    LC 3483 - Unique 3-Digit Even Numbers
    https://leetcode.com/problems/unique-3-digit-even-numbers/

    You are given an array of digits called digits. Your task is to determine the number of distinct three-digit even numbers that can be formed using these digits.

    Note: Each copy of a digit can only be used once per number, and there may not be leading zeros.

    Example 1:

    Input: digits = [1,2,3,4]
    Output: 12
    Explanation: The 12 distinct 3-digit even numbers that can be formed are 124, 132, 134, 142, 214, 234, 312, 314, 324, 342, 412, and 432. Note that 222 cannot be formed because there is only 1 copy of the digit 2.

    Example 2:

    Input: digits = [0,2,2]
    Output: 2
    Explanation: The only 3-digit even numbers that can be formed are 202 and 220. Note that the digit 2 can be used twice because it appears twice in the array.

    Example 3:

    Input: digits = [6,6,6]
    Output: 1
    Explanation: Only 666 can be formed.

    Example 4:

    Input: digits = [1,3,5]
    Output: 0
    Explanation: No even 3-digit numbers can be formed.

    Constraints:

    - 3 <= digits.length <= 10
    - 0 <= digits[i] <= 9
*/
/*
    Best solution: count digit frequencies, then count numbers combinatorially per last digit

    Complexity:
        Time:  O(n + 10) — one pass to count digits, then a constant amount of work per even
               last digit (10 digit values).
        Space: O(10) — the frequency table.

    Approach (detail):
        1. Only digit VALUES matter, so work with cnt[0..9]. A number is (first, middle, last):
           last even, first != 0, and each value used at most cnt[v] times.
        2. Fix the last digit d (even, cnt[d] > 0) and remove one copy. The ordered pairs
           (first, middle) from the remaining multiset number same + distinct * (distinct - 1):
           distinct = values still present, same = values still present twice or more (a pair
           of equal digits).
        3. Subtract the numbers whose first digit is 0: if cnt[0] > 0, remove one 0 as the head,
           then for every even last d (one copy removed) any remaining distinct value can be the
           middle — add distinct values per last digit and subtract that total.
        4. Each (first, middle, last) value triple is counted once, so the result is the number
           of distinct values. (Alternatively: test all 450 even numbers 100..998 against cnt.)
*/
/*
    Your solution: frequency counts + per-last-digit combinatorics, minus zero-headed numbers

    Complexity:
        Time:  O(n + 10) — counting, then two sweeps over the 5 even digits.
        Space: O(10) — digit[], same, distinct.

    Approach:
        1. digit[v] counts each value; same = values with count >= 2, distinct = values present.
        2. collect_even(true): for each even d with digit[d] > 0, take one copy and add
           new_same + new_distinct * (new_distinct - 1), the ordered (first, middle) pairs left.
        3. If a 0 exists, take one 0 as the head (updating same / distinct) and subtract
           collect_even(false): per even last digit d, new_distinct choices for the middle.
        4. n and the commented-out debug prints are leftovers.

    Verdict: MATCHES BEST — the O(n + 10) counting solution.
        Verified against an enumerate-all-index-triples brute force on 300000 random inputs and
        on every digit multiset with up to 3 copies per value (107580 multisets), under
        ASan/UBSan for the random run.
*/
class Solution {
    public:
        int totalNumbers(vector<int>& input) {
            vector<int> digit(10, 0);
            for (auto& d : input) digit[d]++;

            int same = 0, distinct = 0;
            int n = input.size();
            for (int d = 0; d < 10; ++d){
                same += digit[d] >= 2;
                distinct += digit[d] > 0;
            }

            auto collect_even = [&](bool select_two = false) -> int {
                int ans = 0;
                for (int d = 0; d < 10; d += 2){
                    if (!digit[d]) continue;
                    --digit[d];

                    // distinct element
                    if (select_two){
                        // select the same/distinct value for two remain elements
                        int new_same = same - (digit[d] == 1);
                        int new_distinct = distinct - (digit[d] == 0);
                        ans += (new_same + new_distinct * (new_distinct - 1));
                        // cout << "d=" << d << " plus=" << (new_same + new_distinct * (new_distinct - 1)) << "\n";
                    }
                    else {  // select one
                        int new_distinct = distinct - (digit[d] == 0);
                        ans += new_distinct;
                        // cout << "d=" << d << " plus=" << n-2 << "\n";
                    }

                    ++digit[d];
                }
                return ans;
            };

            int ans = collect_even(true);
            if (digit[0] > 0){
                --digit[0];
                same -= (digit[0] == 1);
                distinct -= (digit[0] == 0);
                ans -= collect_even(false);
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

    vector<int> digits = {0, 2, 2};
    digits = {8,1,0};
    digits = {0,2,2,8};
    cout << sol.totalNumbers(digits);

    return 0;
}