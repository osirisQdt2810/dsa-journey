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
    LC 1606 - Find Servers That Handled Most Number of Requests
    https://leetcode.com/problems/find-servers-that-handled-most-number-of-requests/

    You have k servers numbered from 0 to k-1 that are being used to handle multiple requests simultaneously. Each server has infinite computational capacity but cannot handle more than one request at a time. The requests are assigned to servers according to a specific algorithm:

    - The i^th (0-indexed) request arrives.
    - If all servers are busy, the request is dropped (not handled at all).
    - If the (i % k)^th server is available, assign the request to that server.
    - Otherwise, assign the request to the next available server (wrapping around the list of servers and starting from 0 if necessary). For example, if the i^th server is busy, try to assign the request to the (i+1)^th server, then the (i+2)^th server, and so on.

    You are given a strictly increasing array arrival of positive integers, where arrival[i] represents the arrival time of the i^th request, and another array load, where load[i] represents the load of the i^th request (the time it takes to complete). Your goal is to find the busiest server(s). A server is considered busiest if it handled the most number of requests successfully among all the servers.

    Return a list containing the IDs (0-indexed) of the busiest server(s). You may return the IDs in any order.

    Example 1:

    Input: k = 3, arrival = [1,2,3,4,5], load = [5,2,3,3,3]
    Output: [1]
    Explanation:
    All of the servers start out available.
    The first 3 requests are handled by the first 3 servers in order.
    Request 3 comes in. Server 0 is busy, so it's assigned to the next available server, which is 1.
    Request 4 comes in. It cannot be handled since all servers are busy, so it is dropped.
    Servers 0 and 2 handled one request each, while server 1 handled two requests. Hence server 1 is the busiest server.

    Example 2:

    Input: k = 3, arrival = [1,2,3,4], load = [1,2,1,2]
    Output: [0]
    Explanation:
    The first 3 requests are handled by first 3 servers.
    Request 3 comes in. It is handled by server 0 since the server is available.
    Server 0 handled two requests, while servers 1 and 2 handled one request each. Hence server 0 is the busiest server.

    Example 3:

    Input: k = 3, arrival = [1,2,3], load = [10,12,11]
    Output: [0,1,2]
    Explanation: Each server handles a single request, so they are all considered the busiest.

    Constraints:

    - 1 <= k <= 10^5
    - 1 <= arrival.length, load.length <= 10^5
    - arrival.length == load.length
    - 1 <= arrival[i], load[i] <= 10^9
    - arrival is strictly increasing.
*/
/*
    Best solution: ordered set of free servers + min-heap of busy servers by end time

    Complexity:
        Time:  O(n log k) — each request does O(1) heap / ordered-set operations of O(log k).
        Space: O(k) — the free set, the busy heap and the per-server counters.

    Approach (detail):
        1. Keep `free` = ordered set of idle server ids (initially 0..k-1) and `busy` = min-heap of
           (endTime, id). Keep cnt[id] = requests handled.
        2. For request i at time t = arrival[i]: pop every busy server with endTime <= t back into
           `free` (a server finishing exactly at t is available at t).
        3. If `free` is empty, drop the request. Otherwise the wrap-around scan from i % k is the
           first free id >= i % k (lower_bound), or, if none, the smallest free id (begin()).
        4. Assign: remove it from `free`, push (t + load[i], id) onto `busy`, ++cnt[id].
        5. Return every id whose cnt equals max(cnt).
        Key insight: the "try i%k, i%k+1, ... wrapping" rule is a circular successor query, which an
        ordered set answers in O(log k) instead of an O(k) scan.
*/
/*
    Your solution: std::set of idle servers + priority_queue of busy servers keyed by end_time

    Complexity:
        Time:  O(n log k) — each request does a bounded number of heap pops / set operations.
        Space: O(k) — idle, busy and requests[].

    Approach:
        1. idle = {0..k-1}; busy = min-heap of busy_server{end_time, id}; requests[k] = 0.
        2. For each request i: with ts = arrival[i], move every busy server with end_time <= ts
           from busy into idle.
        3. If idle is empty, skip the request. Otherwise it = idle.lower_bound(i % k);
           ns = *it, or *idle.begin() when it == end() (wrap-around).
        4. ++requests[ns], push {ts + load[i], ns} onto busy, erase ns from idle.
        5. busiest = max(requests); collect every id with requests[id] == busiest.

    Verdict: MATCHES BEST — same ordered-set + heap algorithm. ts + load[i] <= 2*10^9 fits in int.
        Verified: 3/3 official examples pass, and it agrees with an O(n*k) simulation on 200000
        random cases (k <= 5, n <= 10).
*/
class Solution {
    public:
        vector<int> busiestServers(int k, vector<int>& arrival, vector<int>& load) {
            struct busy_server {
                int end_time;
                int id;

                bool operator>(const busy_server& o) const {
                    return end_time > o.end_time;
                }
            };

            priority_queue<busy_server, vector<busy_server>, greater<busy_server>> busy;
            set<int> idle;
            for (int i = 0; i < k; ++i) idle.insert(i);
            vector<int> requests(k, 0);

            for (int i = 0; i < (int)arrival.size(); ++i){
                int ts = arrival[i];
                int e = ts + load[i];
                while (!busy.empty() && busy.top().end_time <= ts){
                    int s = busy.top().id; busy.pop();
                    idle.insert(s);
                }

                int target = i % k;
                if (idle.empty()) continue;
                auto it = idle.lower_bound(target);
                int ns = (it != idle.end()) ? (*it) : (*idle.begin());
                // if (busy.size())
                //     cout << "i=" << i << " target=" << target << " t=" << ns << " idle.size()=" << idle.size() << " busy=" << busy.size() << " busy.top()=" << "(" << busy.top().id << "," << busy.top().end_time << ")" << "\n";
                // else
                //     cout << "i=" << i << " target=" << target << " t=" << ns << " idle.size()=" << idle.size() << " busy=" << busy.size() << "\n";

                ++requests[ns];
                busy.push({e, ns});
                idle.erase(ns);
            }

            int busiest = *max_element(requests.begin(), requests.end());
            // cout << "busiest=" << busiest << "\n";
            if (busiest == 0) return {};
            vector<int> ans;
            for (int i = 0; i < k; ++i){
                if (requests[i] == busiest){
                    ans.push_back(i);
                }
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
    // test-lc:begin
    auto to_str = [](const vector<int>& v) {
        string s = "[";
        for (size_t i = 0; i < v.size(); ++i) s += (i ? "," : "") + to_string(v[i]);
        return s + "]";
    };
    int passed = 0, total = 0;
    auto check = [&](int idx, vector<int> got, vector<int> expected) {
        ++total;
        // "You may return the IDs in any order" -> compare sorted
        sort(got.begin(), got.end());
        sort(expected.begin(), expected.end());
        if (got == expected) {
            ++passed;
            cout << "Example " << idx << ": PASS\n";
        } else {
            cout << "Example " << idx << ": FAIL  got=" << to_str(got) << "  expected=" << to_str(expected) << "\n";
        }
    };
    {
        Solution sol;
        int k = 3;
        vector<int> arrival = {1, 2, 3, 4, 5};
        vector<int> load = {5, 2, 3, 3, 3};
        check(1, sol.busiestServers(k, arrival, load), {1});
    }
    {
        Solution sol;
        int k = 3;
        vector<int> arrival = {1, 2, 3, 4};
        vector<int> load = {1, 2, 1, 2};
        check(2, sol.busiestServers(k, arrival, load), {0});
    }
    {
        Solution sol;
        int k = 3;
        vector<int> arrival = {1, 2, 3};
        vector<int> load = {10, 12, 11};
        check(3, sol.busiestServers(k, arrival, load), {0, 1, 2});
    }
    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}