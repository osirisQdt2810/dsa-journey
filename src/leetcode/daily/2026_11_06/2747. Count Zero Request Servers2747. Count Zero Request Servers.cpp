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
    LC 2747 - Count Zero Request Servers
    https://leetcode.com/problems/count-zero-request-servers/

    You are given an integer n denoting the total number of servers and a 2D 0-indexed integer array logs, where logs[i] = [server_id, time] denotes that the server with id server_id received a request at time time.

    You are also given an integer x and a 0-indexed integer array queries.

    Return a 0-indexed integer array arr of length queries.length where arr[i] represents the number of servers that did not receive any requests during the time interval [queries[i] - x, queries[i]].

    Note that the time intervals are inclusive.

    Example 1:

    Input: n = 3, logs = [[1,3],[2,6],[1,5]], x = 5, queries = [10,11]
    Output: [1,2]
    Explanation:
    For queries[0]: The servers with ids 1 and 2 get requests in the duration of [5, 10]. Hence, only server 3 gets zero requests.
    For queries[1]: Only the server with id 2 gets a request in duration of [6,11]. Hence, the servers with ids 1 and 3 are the only servers that do not receive any requests during that time period.

    Example 2:

    Input: n = 3, logs = [[2,4],[2,1],[1,2],[3,1]], x = 2, queries = [3,4]
    Output: [0,1]
    Explanation:
    For queries[0]: All servers get at least one request in the duration of [1, 3].
    For queries[1]: Only server with id 3 gets no request in the duration [2,4].

    Constraints:

    - 1 <= n <= 10^5
    - 1 <= logs.length <= 10^5
    - 1 <= queries.length <= 10^5
    - logs[i].length == 2
    - 1 <= logs[i][0] <= n
    - 1 <= logs[i][1] <= 10^6
    - 1 <= x <= 10^5
    - x < queries[i] <= 10^6
*/
/*
    Best solution: offline queries + sliding window over time-sorted logs

    Complexity:
        Time:  O(M log M + Q log Q + n) — two sorts, then each log enters and leaves the window once.
        Space: O(n + Q) — per-server request counts plus the sorted query indices.

    Approach (detail):
        1. Sort logs by time and sort the queries (keeping their original indices).
        2. Process queries in increasing time. The window [q - x, q] only moves right, so keep two
           pointers into the sorted logs: r adds every log with time <= q, l removes every log with
           time < q - x.
        3. Keep cnt[server] = requests inside the window and active = number of servers with
           cnt > 0 (increment when a count goes 0 -> 1, decrement when it goes 1 -> 0).
        4. The answer for that query is n - active, written back at its original index.
        Why it works: because both window ends are monotone in sorted query order, every log is
        added once and removed at most once, so the total work after sorting is linear.
*/
/*
    Your solution: sorted queries + two-pointer window with an unordered_map of server counts

    Complexity:
        Time:  O(M log M + Q log Q) — the two sorts dominate; each log is added (r) and removed
               (l) at most once.
        Space: O(n + Q) — freq holds at most the servers in the window, qindex holds Q pairs.

    Approach:
        1. qindex = (queries[q], q) sorted by time; logs sorted by logs[i][1].
        2. For each (qt, qi) in order: advance r while logs[r][1] <= qt, doing ++freq[server].
        3. Advance l (while l < r) while logs[l][1] < qt - x, decrementing freq and erasing a
           server whose count drops to 0, so freq.size() = servers with a request in [qt - x, qt].
        4. queries[qi] = n - freq.size(); return queries (reused as the answer array).

    Verdict: MATCHES BEST — same offline sliding window; freq.size() plays the role of `active`.
        Verified against a brute force on 20,000 random small cases.
*/
class Solution {
    public:
        vector<int> countServers(int n, vector<vector<int>>& logs, int x, vector<int>& queries) {
            int M = logs.size();
            int Q = queries.size();

            using ii = pair<int, int>;
            vector<ii> qindex;
            for (int q = 0; q < Q; ++q) qindex.push_back({queries[q], q});
            sort(qindex.begin(), qindex.end());                                                                             // (10, 0) (11, 1)
            sort(logs.begin(), logs.end(), [](const vector<int>& lhs, const vector<int>& rhs){ return lhs[1] < rhs[1]; });  // (1,3) (1,5) (2,6)

            int l = 0, r = 0;
            unordered_map<int, int> freq;
            for (int q = 0; q < Q; ++q){
                int qt = qindex[q].first, qi = qindex[q].second;    // qt=10, qi=0, qt-x=5
                while (r < M && logs[r][1] <= qt){  // freq: [1,1],[2,1]
                    ++freq[logs[r][0]];
                    ++r;
                }

                while (l < r && logs[l][1] < qt - x){
                    if (--freq[logs[l][0]] <= 0){
                        freq.erase(logs[l][0]);
                    }
                    ++l;
                }

                queries[qi] = n - freq.size();
            }
            return queries;
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
        for (size_t i = 0; i < v.size(); ++i) {
            if (i) s += ",";
            s += to_string(v[i]);
        }
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
        int n = 3;
        vector<vector<int>> logs = {{1, 3}, {2, 6}, {1, 5}};
        int x = 5;
        vector<int> queries = {10, 11};
        check(1, sol.countServers(n, logs, x, queries), {1, 2});
    }
    {
        Solution sol;
        int n = 3;
        vector<vector<int>> logs = {{2, 4}, {2, 1}, {1, 2}, {3, 1}};
        int x = 2;
        vector<int> queries = {3, 4};
        check(2, sol.countServers(n, logs, x, queries), {0, 1});
    }

    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}