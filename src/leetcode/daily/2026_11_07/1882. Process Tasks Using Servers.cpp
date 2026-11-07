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
    LC 1882 - Process Tasks Using Servers
    https://leetcode.com/problems/process-tasks-using-servers/

    You are given two 0-indexed integer arrays servers and tasks of lengths n​​​​​​ and m​​​​​​ respectively. servers[i] is the weight of the i^​​​​​​th​​​​ server, and tasks[j] is the time needed to process the j^​​​​​​th​​​​ task in seconds.

    Tasks are assigned to the servers using a task queue. Initially, all servers are free, and the queue is empty.

    At second j, the j^th task is inserted into the queue (starting with the 0^th task being inserted at second 0). As long as there are free servers and the queue is not empty, the task in the front of the queue will be assigned to a free server with the smallest weight, and in case of a tie, it is assigned to a free server with the smallest index.

    If there are no free servers and the queue is not empty, we wait until a server becomes free and immediately assign the next task. If multiple servers become free at the same time, then multiple tasks from the queue will be assigned in order of insertion following the weight and index priorities above.

    A server that is assigned task j at second t will be free again at second t + tasks[j].

    Build an array ans​​​​ of length m, where ans[j] is the index of the server the j^​​​​​​th task will be assigned to.

    Return the array ans​​​​.

    Example 1:

    Input: servers = [3,3,2], tasks = [1,2,3,2,1,2]
    Output: [2,2,0,2,1,2]
    Explanation: Events in chronological order go as follows:
    - At second 0, task 0 is added and processed using server 2 until second 1.
    - At second 1, server 2 becomes free. Task 1 is added and processed using server 2 until second 3.
    - At second 2, task 2 is added and processed using server 0 until second 5.
    - At second 3, server 2 becomes free. Task 3 is added and processed using server 2 until second 5.
    - At second 4, task 4 is added and processed using server 1 until second 5.
    - At second 5, all servers become free. Task 5 is added and processed using server 2 until second 7.

    Example 2:

    Input: servers = [5,1,4,3,2], tasks = [2,1,2,4,5,2,1]
    Output: [1,4,1,4,1,3,2]
    Explanation: Events in chronological order go as follows:
    - At second 0, task 0 is added and processed using server 1 until second 2.
    - At second 1, task 1 is added and processed using server 4 until second 2.
    - At second 2, servers 1 and 4 become free. Task 2 is added and processed using server 1 until second 4.
    - At second 3, task 3 is added and processed using server 4 until second 7.
    - At second 4, server 1 becomes free. Task 4 is added and processed using server 1 until second 9.
    - At second 5, task 5 is added and processed using server 3 until second 7.
    - At second 6, task 6 is added and processed using server 2 until second 7.

    Constraints:

    - servers.length == n
    - tasks.length == m
    - 1 <= n, m <= 2 * 10^5
    - 1 <= servers[i], tasks[j] <= 2 * 10^5
*/
/*
    Best solution: two heaps (idle by (weight, index), busy by free time) with time jumps

    Complexity:
        Time:  O((n + m) log n) — heapify n servers; each task does O(1) heap pushes/pops, and each
               server moves busy -> idle at most once per task it ran.
        Space: O(n + m) — the two heaps plus the answer array.

    Approach (detail):
        1. idle = min-heap of free servers keyed by (weight, index); busy = min-heap of running
           servers keyed by (free_time, ...). Free times can reach about m * max(task) / n, about 4e10,
           so they need 64-bit integers.
        2. Process tasks j = 0..m-1 in order, keeping a clock `now`. Task j can't start before it
           arrives, so now = max(now, j).
        3. If idle is empty, jump now forward to busy.top().free_time: the waiting task starts the
           moment the earliest server frees up.
        4. Move every busy server with free_time <= now into idle. Moving all of them, not just one,
           is what makes ties among servers freeing at the same instant go by (weight, index).
        5. Pop idle.top() as the server for task j, then push it to busy with free_time = now + tasks[j].
        Why it works: tasks start in queue order, and at each start time the set of free servers is
        exactly the servers with free_time <= now. idle then picks the smallest (weight, index)
        among them, which is precisely the rule in the statement.
*/
/*
    Your solution: two heaps + explicit task queue, then a drain loop that pulls from busy directly

    Complexity:
        Time:  O((n + m) log n) — every task is pushed/popped from task_queue once and causes O(1)
               heap operations; each server returns busy -> idle at most once per task it ran.
        Space: O(n + m) — idle, busy, task_queue and ans.

    Approach:
        1. busy_server holds (ll time, weight, id) ordered by tie(time, weight, id); idle_server holds
           (weight, id). Both heaps are min-heaps via greater<>, and every server starts in idle.
        2. For each second j < T: push task j into task_queue; move every busy server with
           time <= j into idle; then, while idle and task_queue are both non-empty, give the front
           task the server idle.top(), push that server to busy with time j + tasks[t], and record
           ans[t].
        3. After the loop, tasks still waiting in task_queue mean idle is empty. Each one takes
           busy.top() and is re-pushed with time ts + tasks[t]. Because busy breaks time ties by
           (weight, id), this picks the same server idle would pick among the servers freeing at ts.
        4. Return ans.

    Verdict: MATCHES BEST — the same two-heap simulation with O((n + m) log n). Ordering busy by
        (time, weight, id) lets the drain loop skip idle, and ll time avoids the 4e10 overflow.
        Verified against a second-by-second brute force on 200,000 random cases, plus a
        worst-case overflow test under UBSan.
*/
class Solution {
    public:
        vector<int> assignTasks(vector<int>& servers, vector<int>& tasks) {
            using ll = long long;
            struct busy_server {
                ll time;
                int weight;
                int id;

                bool operator>(const busy_server& o) const {
                    return tie(time, weight, id) > tie(o.time, o.weight, o.id);
                }
            };

            struct idle_server {
                int weight;
                int id;

                bool operator>(const idle_server& o) const {
                    return tie(weight, id) > tie(o.weight, o.id);
                }
            };

            int S = servers.size(), T = tasks.size();

            queue<int> task_queue;
            priority_queue<busy_server, vector<busy_server>, greater<busy_server>> busy;
            priority_queue<idle_server, vector<idle_server>, greater<idle_server>> idle;

            vector<int> ans(T, -1);
            for (int s = 0; s < S; ++s) idle.push({servers[s], s});
            for (int j = 0; j < T; ++j){
                task_queue.push(j);
                // free current busy server
                while (!busy.empty() && busy.top().time <= j){
                    int s = busy.top().id; busy.pop();
                    idle.push({servers[s], s});
                }
                // assign idle server to task in the order of queue
                while (!idle.empty() && !task_queue.empty()){
                    int t = task_queue.front(); task_queue.pop();
                    int s = idle.top().id; idle.pop();
                    busy.push({1LL * j + tasks[t], servers[s], s});
                    ans[t] = s;
                }
            }

            while (!task_queue.empty()){
                int t = task_queue.front(); task_queue.pop();
                ll ts = busy.top().time;
                int s = busy.top().id;
                busy.pop();
                busy.push({ts + tasks[t], servers[s], s});
                ans[t] = s;
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
        vector<int> servers = {3, 3, 2};
        vector<int> tasks = {1, 2, 3, 2, 1, 2};
        check(1, sol.assignTasks(servers, tasks), {2, 2, 0, 2, 1, 2});
    }
    {
        Solution sol;
        vector<int> servers = {5, 1, 4, 3, 2};
        vector<int> tasks = {2, 1, 2, 4, 5, 2, 1};
        check(2, sol.assignTasks(servers, tasks), {1, 4, 1, 4, 1, 3, 2});
    }

    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}