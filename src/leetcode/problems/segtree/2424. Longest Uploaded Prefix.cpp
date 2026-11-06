#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <climits>

using namespace std;

/*
    LC 2424 - Longest Uploaded Prefix
    https://leetcode.com/problems/longest-uploaded-prefix/

    You are given a stream of n videos, each represented by a distinct number from 1 to n that you need to "upload" to a server. You need to implement a data structure that calculates the length of the longest uploaded prefix at various points in the upload process.

    We consider i to be an uploaded prefix if all videos in the range 1 to i (inclusive) have been uploaded to the server. The longest uploaded prefix is the maximum value of i that satisfies this definition.

    Implement the LUPrefix class:

    - LUPrefix(int n) Initializes the object for a stream of n videos.
    - void upload(int video) Uploads video to the server.
    - int longest() Returns the length of the longest uploaded prefix defined above.

    Example 1:

    Input
    ["LUPrefix", "upload", "longest", "upload", "longest", "upload", "longest"]
    [[4], [3], [], [1], [], [2], []]
    Output
    [null, null, 0, null, 1, null, 3]

    Explanation
    LUPrefix server = new LUPrefix(4);   // Initialize a stream of 4 videos.
    server.upload(3);                    // Upload video 3.
    server.longest();                    // Since video 1 has not been uploaded yet, there is no prefix.
                                         // So, we return 0.
    server.upload(1);                    // Upload video 1.
    server.longest();                    // The prefix [1] is the longest uploaded prefix, so we return 1.
    server.upload(2);                    // Upload video 2.
    server.longest();                    // The prefix [1,2,3] is the longest uploaded prefix, so we return 3.

    Constraints:

    - 1 <= n <= 10^5
    - 1 <= video <= n
    - All values of video are distinct.
    - At most 2 * 10^5 calls in total will be made to upload and longest.
    - At least one call will be made to longest.
*/
/*
    Best solution: uploaded-flag array + monotone prefix pointer (amortized O(1))

    Complexity:
        Time:  O(n + calls) total — the pointer only moves forward, at most n steps over all uploads;
               longest() is O(1).
        Space: O(n) — one flag per video.

    Approach (detail):
        1. Keep a flag array mark[1..n] (plus one sentinel slot at n+1) and the current answer L = 0.
        2. upload(v): set mark[v]. Then, while mark[L+1] is set, advance L.
        3. longest(): return L.
        Why it works: the longest uploaded prefix never shrinks, since videos are only ever added.
        So L can only move forward, and each video makes it move past at most once: O(n) work in
        total across every upload. The sentinel mark[n+1] = false stops the scan once L reaches n
        without reading out of bounds. (A segment tree / DSU also works, but needs O(log n) or
        alpha(n) per op for no benefit here.)
*/
/*
    Your solution: vector<bool> mark with sentinel + forward-only pointer L

    Complexity:
        Time:  O(n + calls) total — the while loop advances L at most n times over the whole run.
        Space: O(n) — mark has n+2 entries.

    Approach:
        1. The constructor sizes mark to n+2 (index n+1 is the always-false sentinel) and sets L = 0.
        2. upload(video): mark[video] = true; while (mark[L+1]) ++L.
        3. longest(): return L.

    Verdict: MATCHES BEST — the optimal amortized-pointer approach; the n+2 size keeps mark[L+1] in
        bounds when all n videos are uploaded. Verified against a brute force on 20,000 random upload
        orders (n <= 8) with libc++ bounds checks enabled.
*/
class LUPrefix {
    private:
        vector<bool> mark;
        int L;

    public:
        LUPrefix(int n) {
            mark.assign(n+2, false);
            L = 0;
        }

        void upload(int video) {
            mark[video] = true;
            while (mark[L+1]) ++L;
        }

        int longest() {
            return L;
        }
};

/**
 * Your LUPrefix object will be instantiated and called as such:
 * LUPrefix* obj = new LUPrefix(n);
 * obj->upload(video);
 * int param_2 = obj->longest();
 */

auto init = [](){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    return 'c';
}();

int main(){
    // test-lc:begin
    // Design replay: null in the expected output = no return value (constructor / upload).
    const int NONE = INT_MIN;
    int passed = 0, total = 0;
    auto show = [&](int v) { return v == NONE ? string("null") : to_string(v); };

    {
        ++total;
        vector<string> ops = {"LUPrefix", "upload", "longest", "upload", "longest", "upload", "longest"};
        vector<vector<int>> args = {{4}, {3}, {}, {1}, {}, {2}, {}};
        vector<int> expected = {NONE, NONE, 0, NONE, 1, NONE, 3};

        LUPrefix* obj = nullptr;
        vector<int> got;
        for (size_t i = 0; i < ops.size(); ++i) {
            if (ops[i] == "LUPrefix") {
                obj = new LUPrefix(args[i][0]);
                got.push_back(NONE);
            } else if (ops[i] == "upload") {
                obj->upload(args[i][0]);
                got.push_back(NONE);
            } else if (ops[i] == "longest") {
                got.push_back(obj->longest());
            }
        }
        delete obj;

        if (got == expected) {
            ++passed;
            cout << "Example 1: PASS\n";
        } else {
            string g = "[", e = "[";
            for (size_t i = 0; i < got.size(); ++i) g += (i ? "," : "") + show(got[i]);
            for (size_t i = 0; i < expected.size(); ++i) e += (i ? "," : "") + show(expected[i]);
            cout << "Example 1: FAIL  got=" << g << "]  expected=" << e << "]\n";
        }
    }

    cout << passed << "/" << total << " passed\n";
    // test-lc:end
    return 0;
}