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
#include <cmath>
#include <tuple>
#include <functional>

using namespace std;

/*
    LC 2243 - Calculate Digit Sum of a String
    https://leetcode.com/problems/calculate-digit-sum-of-a-string/
    @status: self
    @time: 20
    @wa: 1

    You are given a string s consisting of digits and an integer k.

    A round can be completed if the length of s is greater than k. In one round, do the following:

    1. Divide s into consecutive groups of size k such that the first k characters are in the first group, the next k characters are in the second group, and so on. Note that the size of the last group can be smaller than k.
    2. Replace each group of s with a string representing the sum of all its digits. For example, "346" is replaced with "13" because 3 + 4 + 6 = 13.
    3. Merge consecutive groups together to form a new string. If the length of the string is greater than k, repeat from step 1.

    Return s after all rounds have been completed.

    Example 1:

    Input: s = "11111222223", k = 3
    Output: "135"
    Explanation:
    - For the first round, we divide s into groups of size 3: "111", "112", "222", and "23".
      ​​​​​Then we calculate the digit sum of each group: 1 + 1 + 1 = 3, 1 + 1 + 2 = 4, 2 + 2 + 2 = 6, and 2 + 3 = 5.
      So, s becomes "3" + "4" + "6" + "5" = "3465" after the first round.
    - For the second round, we divide s into "346" and "5".
      Then we calculate the digit sum of each group: 3 + 4 + 6 = 13, 5 = 5.
      So, s becomes "13" + "5" = "135" after second round.
    Now, s.length <= k, so we return "135" as the answer.

    Example 2:

    Input: s = "00000000", k = 3
    Output: "000"
    Explanation:
    We divide s into "000", "000", and "00".
    Then we calculate the digit sum of each group: 0 + 0 + 0 = 0, 0 + 0 + 0 = 0, and 0 + 0 = 0.
    s becomes "0" + "0" + "0" = "000", whose length is equal to k, so we return "000".

    Constraints:

    - 1 <= s.length <= 100
    - 2 <= k <= 100
    - s consists of digits only.
*/
/*
    Best solution: direct simulation

    Complexity:
        Time:  O(n) per round, few rounds — a full group of k >= 3 digits shrinks to <= 2 digits; for
               k = 2 a "99" group can stay length 2 once, but the next round's groups "1x" sum to <= 9.
        Space: O(n) for the next string (O(1) extra if rewritten in place).

    Approach (detail):
        1. While s.length() > k: cut s into consecutive groups of k (the last may be shorter), replace each
           group by the decimal string of its digit sum, and concatenate.
        2. Stop as soon as the length is <= k (checked before every round) and return s.
        3. In-place variant: the output of the group starting at i is written to [newsz, newsz + digits)
           with newsz <= i and digits <= group length, so it never overwrites unread input.
*/
/*
    Your solution: in-place simulation over s with a shrinking logical size sz

    Complexity:
        Time:  O(n) per round, few rounds.
        Space: O(1) extra — digits are written back into s.

    Approach:
        1. sz = s.size(); while sz > k: newsz = 0.
        2. For each group start i (step k): sum its digits (j < k and i + j < sz handle the short tail).
        3. d = number of digits - 1 via log10(sum) (0 when sum == 0); write the digits right to left into
           s[newsz .. newsz + d] as characters, then newsz += d + 1 (saved in tmp_d).
        4. sz = newsz; finally return s.substr(0, sz).

    Verdict: MATCHES BEST — the direct simulation, done in place. Counting digits with log10 relies on
        floating point at exact powers of 10 (sum <= 900, so 10 and 100); an integer loop would be safer.
        Verified: 2/2 official examples and a reference simulation on 200000 random strings (n <= 100,
        k <= 100) plus every uniform string, with ASan/UBSan clean.
*/
class Solution {
    public:
        string digitSum(string s, int k) {
            int sz = s.size();
            while (sz > k){
                int newsz = 0;
                for (int i = 0; i < sz; i += k){
                    int sum = 0;
                    for (int j = 0; j < k && i+j < sz; ++j){
                        sum += (int)(s[i+j]-'0');
                    }

                    int d = (sum) ? log10(sum) : 0;
                    int tmp_d = d;
                    while (d >= 0){
                        s[newsz+d] = '0' + (sum % 10);
                        sum /= 10;
                        --d;
                    }
                    newsz += (tmp_d+1);
                }
                sz = newsz;
            }
            return s.substr(0, sz);
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
    auto check = [&](int idx, string got, string expected) {
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
        string s = "11111222223";
        int k = 3;
        check(1, sol.digitSum(s, k), "135");
    }
    {
        Solution sol;
        string s = "00000000";
        int k = 3;
        check(2, sol.digitSum(s, k), "000");
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}
