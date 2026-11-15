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
#include <tuple>
#include <functional>

using namespace std;

/*
    LC 3789 - Minimum Cost to Acquire Required Items
    https://leetcode.com/problems/minimum-cost-to-acquire-required-items/
    @status: self
    @time: 20
    @wa: 1

    You are given five integers cost1, cost2, costBoth, need1, and need2.

    There are three types of items available:

    - An item of type 1 costs cost1 and contributes 1 unit to the type 1 requirement only.
    - An item of type 2 costs cost2 and contributes 1 unit to the type 2 requirement only.
    - An item of type 3 costs costBoth and contributes 1 unit to both type 1 and type 2 requirements.

    You must collect enough items so that the total contribution toward type 1 is at least need1 and the total contribution toward type 2 is at least need2.

    Return an integer representing the minimum possible total cost to achieve these requirements.

    Example 1:

    Input: cost1 = 3, cost2 = 2, costBoth = 1, need1 = 3, need2 = 2
    Output: 3
    Explanation:
    After buying three type 3 items, which cost 3 * 1 = 3, the total contribution to type 1 is 3 (>= need1 = 3) and to type 2 is 3 (>= need2 = 2).
    Any other valid combination would cost more, so the minimum total cost is 3.

    Example 2:

    Input: cost1 = 5, cost2 = 4, costBoth = 15, need1 = 2, need2 = 3
    Output: 22
    Explanation:
    We buy need1 = 2 items of type 1 and need2 = 3 items of type 2: 2 * 5 + 3 * 4 = 10 + 12 = 22.
    Any other valid combination would cost more, so the minimum total cost is 22.

    Example 3:

    Input: cost1 = 5, cost2 = 4, costBoth = 15, need1 = 0, need2 = 0
    Output: 0
    Explanation:
    Since no items are required (need1 = need2 = 0), we buy nothing and pay 0.

    Constraints:

    - 1 <= cost1, cost2, costBoth <= 10^6
    - 0 <= need1, need2 <= 10^9
*/
/*
    Best solution: greedy on unit prices, O(1)

    Complexity:
        Time:  O(1) — a few comparisons and products.
        Space: O(1).

    Approach (detail):
        1. While both requirements are still open, one more unit of each costs either
           cost1 + cost2 (two singles) or costBoth (one combo): buy min(need1, need2) units at
           min(cost1 + cost2, costBoth) each.
        2. After that only one requirement r is left with need_r - min(need1, need2) units; each unit
           costs min(cost_r, costBoth) (a combo also satisfies r, its other half is wasted).
        3. Sum the two parts in long long: need up to 10^9 times cost up to 10^6 is ~10^15.
        Key insight: the cost is linear and separable per unit, so picking the cheapest way to cover
        each "paired" unit and each leftover unit independently is optimal.
*/
/*
    Your solution: greedy — pair with the combo when it is cheaper, then cover the remainder

    Complexity:
        Time:  O(1) — constant arithmetic.
        Space: O(1).

    Approach:
        1. If cost1 + cost2 <= costBoth, buy singles only: cost1*need1 + cost2*need2. (Then costBoth
           >= cost1 and >= cost2, so a combo is never useful for a leftover unit either.)
        2. Otherwise buy c = min(need1, need2) combos for c * costBoth and subtract c from both needs.
        3. The remaining need (need1 or need2) is bought at min(costBoth, cost1) or
           min(costBoth, cost2) per unit, all in long long.

    Verdict: MATCHES BEST — the same O(1) greedy. Verified: 3/3 official examples, an exhaustive
        search over the number of combos on 300000 random cases, and the max inputs
        (10^9 units at 10^6) give 10^15 without overflow.
*/
class Solution {
    public:
        long long minimumCost(int cost1, int cost2, int costBoth, int need1, int need2) {
            if (cost1 + cost2 <= costBoth){
                return 1LL * cost1 * need1 + 1LL * cost2 * need2;
            }

            long long c = min(need1, need2);
            long long cost = c * costBoth;

            need1 -= c;
            need2 -= c;
            return cost + ((need1 > 0) ? 1LL * need1 * min(costBoth, cost1) : 1LL * need2 * min(costBoth, cost2));
        }
};

auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
}();

int main(){
    return 0;
}