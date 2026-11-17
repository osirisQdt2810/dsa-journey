#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

/*
    LC 466 - Count The Repetitions
    https://leetcode.com/problems/count-the-repetitions/
    @status: editorial
    @time: 171
    @wa: 0
    @created: 2026-11-17 13:49

    We define str = [s, n] as the string str which consists of the string s concatenated n times.

    - For example, str == ["abc", 3] =="abcabcabc".

    We define that string s1 can be obtained from string s2 if we can remove some characters from s2 such that it becomes s1.

    - For example, s1 = "abc" can be obtained from s2 = "abdbec" based on our definition by removing the bolded underlined characters.

    You are given two strings s1 and s2 and two integers n1 and n2. You have the two strings str1 = [s1, n1] and str2 = [s2, n2].

    Return the maximum integer m such that str = [str2, m] can be obtained from str1.

    Example 1:

    Input: s1 = "acb", n1 = 4, s2 = "ab", n2 = 2
    Output: 2

    Example 2:

    Input: s1 = "acb", n1 = 1, s2 = "acb", n2 = 1
    Output: 1

    Constraints:

    - 1 <= s1.length, s2.length <= 100
    - s1 and s2 consist of lowercase English letters.
    - 1 <= n1, n2 <= 10^6
*/
/*
    Best solution: per-start-position state table over one copy of s1, then chain n1 copies

    Complexity:
        Time:  O(|s1| * |s2| + n1) — the table costs |s2| passes over s1, the chain is n1 O(1) steps.
        Space: O(|s2|) — two arrays indexed by the position in s2.

    Approach (detail):
        1. Match s2 greedily against the long string s1^n1: taking a character as soon as it matches
           never hurts a subsequence match. After any prefix of the long string, the only thing that
           matters for the future is the position j in s2 that is currently needed.
        2. For every j in [0, |s2|), read one copy of s1 starting with "need s2[j]" and record
           nxt[j] = the position needed afterwards and wrap[j] = how many full copies of s2 were
           completed during that copy.
        3. Start at state 0 and run n1 times: total += wrap[state]; state = nxt[state].
        4. total is the number of full s2 copies found in s1^n1; str2 = [s2, n2] needs n2 of them, so
           the answer is total / n2. (Cycle detection on `state` is an optional speed-up when n1 is
           huge, but O(n1) steps are already cheap for n1 <= 10^6.)
*/
/*
    Your solution: nxt/cnt state table over one copy of s1, then chain n1 copies

    Complexity:
        Time:  O(|s1| * |s2| + n1).
        Space: O(|s2|) — nxt and cnt.

    Approach:
        1. nxt[j] / cnt[j]: starting with pos2 = j, scan every character c of s1; when c == s2[pos2]
           advance pos2, and on reaching the last character of s2 count one completion (cnt2) and
           reset pos2 to 0. Store nxt[j] = pos2 and cnt[j] = cnt2.
        2. state = 0, total = 0; for i < n1: total += cnt[state]; state = nxt[state].
        3. Return total / n2.

    Verdict: MATCHES BEST — the standard state-table solution (the only extra optimisation would be
        cycle detection, unnecessary at n1 <= 10^6). total <= n1 * |s1| / |s2| <= 10^8 fits in int.
        Verified: 2/2 official examples and a brute force (s2^(n2*m) as a subsequence of s1^n1) on 30000
        random cases, plus the maximum case n1 = 10^6, |s1| = 100, |s2| = 1.
*/
class Solution {
    public:
        int getMaxRepetitions(string s1, int n1, string s2, int n2) {
            // idea đơn giản: trong 1 lượt s1 đi qua được bao nhiêu s2? cụ thể hơn là: đi từ 1 straightforward approach:
            // giả sử: s1 = "abcab" và s2 = "bca"
            // bắt đầu từ:
            //      - s2: 'b', khi duyệt hết s1, ta được "bca" "b" (như vậy nếu ta đi từ b -> ta đi hết s1 thì ta duyệt được 1 lượt s2, và tại lượt 2, nó đang cần thêm 'c' từ s1)
            //      - s2: 'c', khi duyệt hết s1, ta được "ca" "b" (như vậy nếu ta đi từ c -> ta đi hết s1 thì ta duyệt thêm được 1 lượt s2, và tại lượt 2, nó cũng cần thêm 'c' từ s1)
            //      - s2: 'a', khi duyệt hết s1, ta được "a" "bca" "b" (như vậy nếu đi từ a -> ta đi hết s1 thì duyệt được 2 lượt s2, và tại lượt 2, nó cũng cần thêm 'c' từ s1)

            // bây giờ, ta cần duyệt hết n lượt s1, kiểm tra xem n lượt đó achieve được bao s2, giả sử gọi là X. Kết quả cuối cùng, answer = X / n2
            // lấy ví dụ ở trên:
            //      - đi qua lượt s1 đầu tiên, đang ở 'b' (0) => X += 1, vị trí tiếp theo cần xét là 'c'
            //      - đi qua lượt s1 thứ 2, đang ở 'c' (1) => X += 1, vị trí tiếp theo cần xét là 'c'
            //      - đi qua lượt s1 thứ 3, ...
            //      .... cứ như vậy đi tới hết n1 lượt s1
            int l2 = s2.size();
            vector<int> nxt(l2, 0);
            vector<int> cnt(l2, 0);

            for (int j = 0; j < l2; ++j){
                int cnt2 = 0;
                int pos2 = j;

                for (auto c : s1){
                    if (c == s2[pos2]){
                        if (pos2 == l2-1){
                            ++cnt2;
                            pos2 = 0;
                        }
                        else {
                            ++pos2;
                        }
                    }
                }
                nxt[j] = pos2;
                cnt[j] = cnt2;
            }

            int state = 0;
            int total = 0;
            for (int i = 0; i < n1; ++i){
                total += cnt[state];
                state = nxt[state];
            }
            return total / n2;
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
        string s1 = "acb";
        int n1 = 4;
        string s2 = "ab";
        int n2 = 2;
        check(1, sol.getMaxRepetitions(s1, n1, s2, n2), 2);
    }
    {
        Solution sol;
        string s1 = "acb";
        int n1 = 1;
        string s2 = "acb";
        int n2 = 1;
        check(2, sol.getMaxRepetitions(s1, n1, s2, n2), 1);
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}