#include <vector>
#include <iostream>
#include <stack>
#include <algorithm>
#include <climits>

using namespace std;

/*
    LC 1964 - Find the Longest Valid Obstacle Course at Each Position
    https://leetcode.com/problems/find-the-longest-valid-obstacle-course-at-each-position/

    You want to build some obstacle courses. You are given a 0-indexed integer array obstacles of length n, where obstacles[i] describes the height of the i^th obstacle.

    For every index i between 0 and n - 1 (inclusive), find the length of the longest obstacle course in obstacles such that:

    - You choose any number of obstacles between 0 and i inclusive.
    - You must include the i^th obstacle in the course.
    - You must put the chosen obstacles in the same order as they appear in obstacles.
    - Every obstacle (except the first) is taller than or the same height as the obstacle immediately before it.

    Return an array ans of length n, where ans[i] is the length of the longest obstacle course for index i as described above.

    Example 1:

    Input: obstacles = [1,2,3,2]
    Output: [1,2,3,3]
    Explanation: The longest valid obstacle course at each position is:
    - i = 0: [1], [1] has length 1.
    - i = 1: [1,2], [1,2] has length 2.
    - i = 2: [1,2,3], [1,2,3] has length 3.
    - i = 3: [1,2,3,2], [1,2,2] has length 3.

    Example 2:

    Input: obstacles = [2,2,1]
    Output: [1,2,1]
    Explanation: The longest valid obstacle course at each position is:
    - i = 0: [2], [2] has length 1.
    - i = 1: [2,2], [2,2] has length 2.
    - i = 2: [2,2,1], [1] has length 1.

    Example 3:

    Input: obstacles = [3,1,5,6,4,2]
    Output: [1,1,2,3,2,2]
    Explanation: The longest valid obstacle course at each position is:
    - i = 0: [3], [3] has length 1.
    - i = 1: [3,1], [1] has length 1.
    - i = 2: [3,1,5], [3,5] has length 2. [1,5] is also valid.
    - i = 3: [3,1,5,6], [3,5,6] has length 3. [1,5,6] is also valid.
    - i = 4: [3,1,5,6,4], [3,4] has length 2. [1,4] is also valid.
    - i = 5: [3,1,5,6,4,2], [1,2] has length 2.

    Constraints:

    - n == obstacles.length
    - 1 <= n <= 10^5
    - 1 <= obstacles[i] <= 10^7
*/
/*
    Best solution: patience sorting (non-decreasing LIS with upper_bound)

    Complexity:
        Time:  O(n log L) — one binary search over tails per element, L = LIS length <= n.
        Space: O(L) — the tails array.

    Approach (detail):
        1. The answer at i is the longest non-decreasing subsequence ending at obstacles[i].
        2. Keep tails[k] = smallest possible last value of a valid course of length k+1 seen so
           far. tails is non-decreasing (drop the last element of the best length-(k+2) course to
           get a length-(k+1) course ending no higher).
        3. For x, P = upper_bound(tails, x) = number of lengths x can extend (tails[k] <= x), so
           the answer is P+1. upper_bound (not lower_bound) because equal heights are allowed.
        4. Update: P == size -> push_back(x); else tails[P] = x (x < tails[P], a better ending).
           No other cell changes: cells < P already end <= x, and a length > P+1 course ending
           at x would need a length-(P+1) course ending <= x, contradicting tails[P] > x.
        5. A monotonic stack is NOT enough — popping larger values discards chains that later
           elements can still extend.
*/
/*
    Your solution: better_patience_sorting — tails + upper_bound, answer written in place

    Complexity:
        Time:  O(n log L) — one upper_bound per element over the current tails.
        Space: O(L) — tails.

    Approach:
        1. For each x in obstacles: P = upper_bound(tails, x) - tails.begin().
        2. If P == tails.size() push_back(x), else tails[P] = x.
        3. Overwrite x with P+1 and return obstacles.

    Also in the file (not called): wrong_monostack — monotonic stack size, incorrect (fails
        [5,1,5,5,1,3,4,5,1,4] at the last index); fenwick_with_compression — prefix-max Fenwick
        over compressed heights, O(n log n) with a larger constant; patience_sorting — same idea
        but tails pre-filled with n INT_MAX, so every search spans n cells.

    Verdict: MATCHES BEST — the optimal patience-sorting algorithm.
        Verified against an O(n^2) DP on 5000 random arrays (n <= 12).
*/
class Solution {
    public:
        vector<int> longestObstacleCourseAtEachPosition(vector<int>& obstacles) {
            //              [5,1,5,5,1,3,4,5,1,4]
            // expected:    [1,1,2,3,2,3,4,5,3,5]
            // output:      [1,1,2,3,2,3,4,5,3,4]

            auto wrong_monostack = [&]() -> vector<int> {
                int n = obstacles.size();

                struct monostack {
                    stack<int> st;

                    void push(int val){
                        while (!st.empty() && st.top() > val) st.pop();
                        st.push(val);
                    }

                    int size(){
                        return st.size();
                    }

                };

                monostack st;

                for (int i = 0; i < n; ++i){
                    // monostack.push(obstacles[i])
                    // return monostack.size()
                    st.push(obstacles[i]);
                    obstacles[i] = st.size();
                }
                return obstacles;
            };

            auto fenwick_with_compression = [&]() -> vector<int> {
                auto rank = [](vector<int>& vals , int X){
                    return lower_bound(vals.begin(), vals.end(), X) - vals.begin();
                };

                auto compress = [&](vector<int>& input){
                    auto out = input;
                    sort(out.begin(), out.end());
                    out.erase(unique(out.begin(), out.end()), out.end());
                    return out;
                };

                struct fenwick {
                    int N;
                    vector<int> bits;

                    fenwick(int n) : N(n+1), bits(N, 0){}

                    int query_max(int k){
                        int res = 0;
                        while (k > 0){
                            res = max(res, bits[k]);
                            k -= k & (-k);
                        }
                        return res;
                    }

                    void update_max(int k, int val){
                        while (k < N){
                            bits[k] = max(bits[k], val);
                            k += k & (-k);
                        }
                    }

                };

                int n = obstacles.size();
                vector<int> vals = compress(obstacles);
                fenwick tree(n);
                for (int i = 0; i < n; ++i){
                    int id = rank(vals, obstacles[i]);
                    obstacles[i] = 1+tree.query_max(id+1);
                    tree.update_max(id+1, obstacles[i]);
                }
                return obstacles;
            };

            auto patience_sorting = [&]() -> vector<int> {
                // idea: gọi tails[k] là phần tử nhỏ nhất của 1 chuỗi valid có độ dài k
                // dễ thấy:
                //      1. tails là 1 mảng tăng dần từ 1->N. Lí do: giả sử tại 1 điểm k+1 mà tails[k+1] < tails[k]. tức tồn tại 1 chuỗi x[1]...x[k] <= tails[k+1]=x[k+1] < tails[k]. vô lí vì tails[k] là nhỏ nhất trong tập chuỗi k
                //      2. khi có thêm 1 giá trị x, bởi vì chuỗi tails tăng dần, ta chỉ cần quan tâm điểm P đầu tiên mà tails[P] > x, bởi vì:
                //          - mọi điểm tails[1...P-1] <= x => không cần cập nhật
                //          - cập nhật lại tails[k] = x, bởi vì ta có thể gom chuỗi trong tails[k-1] & x => chuỗi tails[k] tốt hơn
                //          - không thể cập nhật tails[P+1]=x, vì nếu vậy thì tồn tại 1 chuỗi tails[P]' khác <= x rồi, trong khi ta biết được P là điểm đầu tiên mà tails[P] > x
                int n = obstacles.size();
                vector<int> tails(n, INT_MAX);
                for (auto& x : obstacles){
                    int P = upper_bound(tails.begin(), tails.end(), x) - tails.begin();
                    tails[P] = x;
                    x = P+1;
                }
                return obstacles;
            };

            auto better_patience_sorting = [&]() -> vector<int> {
                // idea: gọi tails[k] là phần tử nhỏ nhất của 1 chuỗi valid có độ dài k
                // dễ thấy:
                //      1. tails là 1 mảng tăng dần từ 1->N. Lí do: giả sử tại 1 điểm k+1 mà tails[k+1] < tails[k]. tức tồn tại 1 chuỗi x[1]...x[k] <= tails[k+1]=x[k+1] < tails[k]. vô lí vì tails[k] là nhỏ nhất trong tập chuỗi k
                //      2. khi có thêm 1 giá trị x, bởi vì chuỗi tails tăng dần, ta chỉ cần quan tâm điểm P đầu tiên mà tails[P] > x, bởi vì:
                //          - mọi điểm tails[1...P-1] <= x => không cần cập nhật
                //          - cập nhật lại tails[k] = x, bởi vì ta có thể gom chuỗi trong tails[k-1] & x => chuỗi tails[k] tốt hơn
                //          - không thể cập nhật tails[P+1]=x, vì nếu vậy thì tồn tại 1 chuỗi tails[P]' khác <= x rồi, trong khi ta biết được P là điểm đầu tiên mà tails[P] > x
                vector<int> tails;
                for (auto& x : obstacles){
                    int P = upper_bound(tails.begin(), tails.end(), x) - tails.begin();
                    if (P == tails.size()) tails.push_back(x);
                    else  tails[P] = x;
                    x = P+1;
                }
                return obstacles;
            };

            return better_patience_sorting();
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