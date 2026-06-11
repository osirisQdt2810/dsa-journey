#include <vector>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>
#include <queue>

using namespace std;

class Solution {
    private:
        struct point {
            int l;
            int r;

            bool valid(int n){
                return l >= 1 && r <= n && l <= r;
            }
        };

        struct node {
            int val;
            point p;
        };

        struct range_max_min_query {
            struct info {
                int mmin{INT_MAX};
                int mmax{INT_MIN};

                info operator+(const info& other){
                    return {
                        min(mmin, other.mmin),
                        max(mmax, other.mmax),
                    };
                }

                int operator-(const info& other){
                    return max(mmax, other.mmax) - min(mmin, other.mmin);
                }
            };

            int n;
            int klog;
            vector<vector<info>> nodes; // nodes[u][i]: info trong khoảng [u, u+2^i)
                                        // nodes[u][i]: info(nodes[u][i-1], nodes[u+2^i][i-1])

            range_max_min_query(const vector<int>& arr) : n(arr.size()), klog(floor(log2(n))){
                nodes.resize(n+1, vector<info>(klog+1));
                for (int u = 1; u <= n; ++u){
                    nodes[u][0] = nodes[u][0] + info({arr[u-1], arr[u-1]});
                    // cout << "u: " << u << ", i: 0, nodes[u][i]: [" << nodes[u][0].mmin << ", " << nodes[u][0].mmax << "]" << endl;
                }
                // 1 3 2
                for (int i = 1; i <= klog; ++i){
                    for (int u = 1; u <= n; ++u){
                        int r = u + (1 << (i-1));
                        info rinfo = (r <= n) ? nodes[r][i-1] : info();
                        nodes[u][i] = nodes[u][i-1] + rinfo;
                        // cout << "u: " << u << ", i: " << i << ", nodes[u][i]: [" << nodes[u][i].mmin << ", " << nodes[u][i].mmax << "]" << " -extra: r=" << r << endl;
                    }
                }
            }

            int query(point pts){
                cout << "l: " << pts.l << ", r: " << pts.r << endl;
                int l = pts.l, r = pts.r;
                int k = floor(log2(r-l+1));
                return nodes[l][k] - nodes[r-(1<<k)+1][k];
            }
        };

    public:
        long long maxTotalValue(vector<int>& nums, int k) {
            int n = nums.size();
            range_max_min_query rmq(nums);

            auto cmp = [](const node& lhs, const node& rhs){
                return lhs.val < rhs.val;   // max-heap on val
            };
            auto index = [&](const point& pts){
                return pts.l * n + pts.r;
            };

            priority_queue<node, vector<node>, decltype(cmp)> pq(cmp);
            unordered_set<int> visited;
            int cnt = 0;
            point start = {1, n};
            pq.push({rmq.query(start), start});
            visited.insert(index(start));

            // for (int l = 1; l <= n; ++l){
            //     for (int r = l; r <= n; ++r){
            //         point pts = {l, r};
            //         cout << "pts: [" << pts.l << ", " << pts.r << "], val: " << rmq.query(pts) << endl;
            //     }
            // }

            long long score = 0;
            while (cnt < k){
                node top = pq.top(); pq.pop();
                int l = top.p.l, r = top.p.r;
                
                score += top.val;
                cnt++;

                // left
                point left = {l, r-1};
                int lindex = index(left);
                if (left.valid(n) && !visited.count(lindex)){
                    pq.push({rmq.query(left), left});
                    visited.insert(lindex);
                }

                // bottom
                point bottom = {l+1, r};
                int bindex = index(bottom);
                if (bottom.valid(n) && !visited.count(bindex)){
                    pq.push({rmq.query(bottom), bottom});
                    visited.insert(bindex);
                }
            }
            return score;
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
    vector<int> nums = {1,3,2};
    nums = {4,2,5,1};
    nums = {11,8};
    int k = 2;

    cout << sol.maxTotalValue(nums, k) << endl;

    return 0;
}